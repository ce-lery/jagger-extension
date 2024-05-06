# jagger-extension

[![BSD-2-Clause](https://custom-icon-badges.herokuapp.com/badge/license-BSD%202%20Clause-8BB80A.svg?logo=law&logoColor=white)](LICENSE)
![C++](https://custom-icon-badges.herokuapp.com/badge/C++-f34b7d.svg?logo=Cplusplus&logoColor=white)
![Linux](https://custom-icon-badges.herokuapp.com/badge/Linux-F6CE18.svg?logo=Linux&logoColor=white)

## Overview 

This repository is the extension kit for [jagger-2023-02-18](https://www.tkl.iis.u-tokyo.ac.jp/~ynaga/jagger/).

> Naoki Yoshinaga  
> [Back to Patterns: Efficient Japanese Morphological Analysis with Feature-Sequence Trie](https://aclanthology.org/2023.acl-short.2/)  
> The 61st Annual Meeting of the Association for Computational Linguistics (ACL-23). Toronto, Canada. July 2023  

In this repository, in order to improve the efficiency of using jagger's morphological analysis results, 
jagger.cc has been improved to return a list of morphemes from the analysis results as a vector.  
This may cause a slight decrease in processing speed performance (untested). 

The changes is following.

- **jagger-extension/jagger.cc**: Add DivideMorpheme().
- **jagger-extension/jagger.h**: Add prototype declaration of Jagger class.
- **jagger-extension/ccedar_core.h, ccedar_core.cc**: 
    - Because there was a memory leak, the argument of the clear function on line 51 was changed to false.
    - Because link error, change line16 to function prototype declaration. The substance is in the ccedar_core.cc

## Getting Started
### Docker

Build a python environment using Docker files.

```bash
docker build -t jagger-extension-image $PWD
docker run -v $PWD:/home/jagger-extension/ -it --gpus all jagger-extension-image
```

### jagger build

```bash
bash scripts/build.sh
```

### Run

```bash
bash scripts/run.sh

# ~ommit~
# あ              感動詞,*,*,*,あ,あ,*
# …               特殊,記号,*,*,…,…,*
# ありのまま              副詞,*,*,*,ありのまま,ありのまま,代表表記:ありのまま/ありのまま
#                 名詞,普通名詞,*,*,*.*,*
# 今              名詞,時相名詞,*,*,今,いま,*
# 起こった                動詞,*,子音動詞ラ行,タ形,起こる,おこった,代表表記:起こる/おこる 自他動詞:他:起こす/おこす
# 事              名詞,普通名詞,*,*,事,こと,*
# を              助詞,格助詞,*,*,を,を,*
# 話す            動詞,*,子音動詞サ行,基本形,話す,はなす,代表表記:話す/はなす 補文ト
# ぜ              助詞,終助詞,*,*,ぜ,ぜ,*
# ！              特殊,記号,*,*,！,！,*
```

## License

This repository is licensed under the [The 2-Clause BSD License](LICENSE).  

## TODO

- [ ] Debug read of jagger.h
    - When checking with valgrind, under certain conditions, it becomes Invalid read of size 1.



