#include <string>
#include <vector>
#include <jagger.h>
#include <iostream>

// Jagger -- deterministic pattern-based Japanese tagger
//  $Id: jagger.cc 2031 2023-02-17 21:47:05Z ynaga $
// Copyright (c) 2022 Naoki Yoshinaga <ynaga@iis.u-tokyo.ac.jp>
// #include <jagger.h>


Jagger::Jagger () : da (), c2i (0), p2f (0), fs (0), mmaped () {}

Jagger::~Jagger () {
    for (size_t i = 0; i < mmaped.size (); ++i)
    ::munmap (mmaped[i].first, mmaped[i].second);
}

template <typename T>
inline void Jagger::write_array (T& data, const std::string& fn) {
    FILE *fp = std::fopen (fn.c_str (), "wb");
    if (! fp) errx (1, "no such file: %s", fn.c_str ());
    std::fwrite (&data[0], sizeof (typename T::value_type), data.size (), fp);
    std::fclose (fp);
}
void* Jagger::read_array (const std::string& fn) {
    int fd = ::open (fn.c_str (), O_RDONLY);
    if (fd == -1) errx (1, "no such file: %s", fn.c_str ());
    // get size and read;
    const size_t size = ::lseek (fd, 0, SEEK_END);
    ::lseek (fd, 0, SEEK_SET);
    void *data = ::mmap (0, size, PROT_READ, MAP_SHARED, fd, 0);
    ::close (fd);
    mmaped.push_back (std::make_pair (data, size));
    return data;
}

void Jagger::read_model (const std::string& m) { // read patterns to memory
  const std::string da_fn (m + ".da"), c2i_fn (m + ".c2i"), p2f_fn (m + ".p2f"), fs_fn (m + ".fs");
  struct stat st;
  if (::stat (da_fn.c_str (), &st) != 0) { // compile
    std::fprintf (stderr, "building DA trie from patterns..");
    std::vector <uint16_t> c2i_; // mapping from utf8, BOS, unk to char ID
    std::vector <uint64_t> p2f_; // mapping from pattern ID to feature str
    std::vector <char>      fs_; // feature strings
    sbag_t fbag ("\tBOS");
    sbag_t fbag_ ((std::string (FEAT_UNK) + ",*,*,*\n").c_str ());
    std::map <uint64_t, int> fs2pid;
    fs2pid.insert (std::make_pair ((1ul << 32) | 2, fs2pid.size ()));
    p2f_.push_back ((1ul << 32) | 2);
    // count each character to obtain dense mapping
    std::vector <std::pair <size_t, int> > counter (CP_MAX + 3);
    for (int u = 0; u < counter.size (); ++u) // allow 43 bits for counting
      counter[u] = std::make_pair (0, u);
    std::vector <std::pair <std::string, uint64_t > > keys;
    char *line = 0;
    simple_reader reader (m.c_str ());
    while (const size_t len = reader.gets (&line)) { // find pos offset
      // pattern format: COUNT PATTEN PREV_POS BYTES CHAR_TYPE FEATURES
      char *p (line), * const p_end (p + len);
      const size_t count = std::strtoul (p, &p, 10);
      const char *pat = ++p;
      for (int b = 0; *p != '\t'; p += b)
        counter[unicode (p, b)].first += count + 1;
      size_t fi_prev = 0;
      const char* f_prev = p; // starting with '\t'
      if (*++p != '\t') { // with pos context
        p = const_cast <char*> (skip_to (p, 1, '\t')) - 1;
        fi_prev = fbag.to_i (f_prev, p - f_prev) + 1;
        if (fi_prev + CP_MAX == counter.size ()) // new part-of-speech
          counter.push_back (std::make_pair (0, (fi_prev + CP_MAX)));
        counter[fi_prev + CP_MAX].first += count + 1;
      }
      const size_t bytes = std::strtoul (++p, &p, 10);
      const size_t ctype = std::strtoul (++p, &p, 10);
      const char* f = p; // starting with '\t'
      p = const_cast <char*> (skip_to (p, NUM_POS_FIELD, ',')) - 1;
      const size_t fi_  = fbag.to_i  (f, p - f) + 1;

      const size_t fi = fbag_.to_i (p, p_end - p) + 1;
      if (fi_ + CP_MAX == counter.size ()) // new part-of-speech
        counter.push_back (std::make_pair (0, fi_ + CP_MAX));
      std::pair <std::map <uint64_t, int>::iterator, bool> itb
        = fs2pid.insert (std::make_pair ((fi << 32) | fi_, fs2pid.size ()));
      if (itb.second) p2f_.push_back ((fi << 32) | fi_);
      keys.push_back (std::make_pair (std::string (pat, f_prev - pat),
                                      (((bytes << 23) | ((ctype & 0x7) << 20) | (itb.first->second & 0xfffff)) << 12) | fi_prev));
    }
    // save c2i
    std::sort (counter.begin () + 1, counter.end (), std::greater <std::pair <size_t, int> > ());
    c2i_.resize (counter.size ());
    for (unsigned int i = 1; i < counter.size () && counter[i].first; ++i)
      c2i_[counter[i].second] = static_cast <uint16_t> (i);
    // save feature strings
    std::vector <size_t> offsets;

    fbag_.serialize (fs_, offsets);
    write_array (fs_, fs_fn);
    // save mapping from morpheme ID to morpheme feature strings
    for (size_t i = 0; i < p2f_.size (); ++i) {

      const std::string& f = fbag_.to_s ((p2f_[i] >> 32) - 1);
      const char* q = skip_to (f.c_str (), NUM_POS_FIELD, ',') - 1;
      p2f_[i] = (offsets[(p2f_[i] >> 32) - 1] << 34) |
                (fbag_.to_s ((p2f_[i] >> 32) - 1).size () << (MAX_KEY_BITS + MAX_FEATURE_BITS)) |
                (q - f.c_str ()) << MAX_KEY_BITS |
                c2i_[(p2f_[i] & 0xffffffff) + CP_MAX];
    }
    write_array (p2f_, p2f_fn);
    // save pattern trie
    for (std::vector <std::pair <std::string, uint64_t> >::const_iterator it = keys.begin (); it != keys.end (); ++it) {
      std::vector <int> key;
      for (int offset (0), b (0); offset < it->first.size (); offset += b)
        key.push_back (c2i_[unicode (&it->first[offset], b)]);
      if (it->second & 0xfff)
        key.push_back (c2i_[(it->second & 0xfff) + CP_MAX]);
      da.update (&key[0], key.size ()) = it->second >> 12;
    }
    c2i_.resize (CP_MAX + 2); // chop most of part-of-speech mapping
    write_array (c2i_, c2i_fn);
    da.save (da_fn.c_str ());
    std::fprintf (stderr, "done.\n");
  }
  da.set_array (read_array (da_fn));
  c2i = static_cast <uint16_t*> (read_array (c2i_fn));
  p2f = static_cast <uint64_t*> (read_array (p2f_fn));
  fs  = static_cast <char*> (read_array (fs_fn));
}

void Jagger::DivideMorpheme(const std::string sentence, std::vector<std::string> &morpheme, std::vector<std::string> &morpheme_form) 
{
    size_t len = (size_t)sentence.size();
    char* line = new char[sentence.length() + 1]; // null終端文字分の+1が必要
    strcpy(line, sentence.c_str());

    int bytes (0), bytes_prev (0), id (0), ctype (0), ctype_prev (0);
    uint64_t offsets = c2i[CP_MAX + 1];
    bool bos (true),ret (sentence[len - 1] == '\n'), concat (false);
    size_t morpheme_form_size;
    std::string morpheme_concat="";

    for (const char *p (line), * const p_end (p + len - ret); p != p_end; bytes_prev = bytes, ctype_prev = ctype, offsets = p2f[static_cast <size_t> (id)], p += bytes) {
        
        const int r = da.longestPrefixSearchWithPOS (p, p_end, offsets & 0x3fff, &c2i[0]); // found word
        id    = r & 0xfffff;
        bytes = (r >> 23) ? (r >> 23) : u8_len (p);
        ctype = (r >> 20) & 0x7; // 0: num|unk / 1: alpha / 2: kana / 3: other
        if (! bos) { // word that may concat with the future context
            if (ctype_prev != ctype || // different character types
                ctype_prev == 3 ||     // seen words in non-num/alpha/kana
                (ctype_prev == 2 && bytes_prev + bytes >= 18)) {

                if (concat) morpheme_form_size =  (offsets >> MAX_KEY_BITS) & 0x7f;
                else        morpheme_form_size = (offsets >> (MAX_KEY_BITS + MAX_FEATURE_BITS)) & 0x3ff;

                std::string s_temp(reinterpret_cast<char*>(&fs[(offsets >> 34)]), morpheme_form_size-1);
                morpheme_form.push_back(s_temp.substr(1)); // Exclude because the first character contains \t
                // std::cout << "品詞:" << s_temp.substr(1) << std::endl; 
                concat = false;
            } 
            else    concat = true;
        } 
        // else    bos = false;

        std::string s_temp(p, p + static_cast<size_t>(bytes));
        // std::cout << "形態素:" << s_temp << std::endl;

        if(bos){
            morpheme_concat+=s_temp;
            bos = false;
            continue;
        }

        if(concat)  morpheme_concat+=s_temp;
        else    {
            // std::cout << "morpheme_concat:" << morpheme_concat << std::endl;
            morpheme.push_back(morpheme_concat);
            morpheme_concat=s_temp;
        }
        // std::cout << "形態素:" << s_temp << std::endl;

    }

    if (concat) morpheme_form_size =  (offsets >> MAX_KEY_BITS) & 0x7f;            
    else        morpheme_form_size = (offsets >> (MAX_KEY_BITS + MAX_FEATURE_BITS)) & 0x3ff;
    
    morpheme.push_back(morpheme_concat);

    std::string s_temp(reinterpret_cast<char*>(&fs[(offsets >> 34)]), morpheme_form_size-1);
    // std::cout << "品詞:" << s_temp.substr(1) << std::endl; 
    morpheme_form.push_back(s_temp.substr(1));
    concat = false;            
    delete[] line; 
}




