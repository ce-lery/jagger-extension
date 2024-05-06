#include <iostream>
#include <string>
#include <vector>
#include <sstream>  // この行を追加
#include <chrono>  
#include <jagger.h>
// #include "ccedar_core.h"
// #include "config.h"

using namespace std;


int main(void) 
{
    string model ("jagger-2023-02-18/model/kwdlc/patterns");

    // read jagger model
    Jagger jagger_parser;
    jagger_parser.read_model(model);

    chrono::system_clock::time_point start, end;
    uint32_t elapsed_time=0;

    vector<string> morpheme,morpheme_form;
    string sentence = "私は香川出身1のAで、超有名で、約5年です";
    jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);

    start = chrono::system_clock::now();

    // for(int i=0;i<100;i++){
    //   sentence = "私は香川出身1のAで、超有名で、約5年です";
    //   jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);
    //   end = std::chrono::system_clock::now();
    //   cout << "elapsed_time(ms):" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<endl;
    //   start=end;
    // }

    // sentence = "1868";
    // jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);

    sentence = "私は香川出身1のAで、超有名で、約5年です";
    jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);
    end = std::chrono::system_clock::now();
    cout << "elapsed_time(ms):" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<endl;
    start=end;

    sentence = "日本における東洋史の概念は、帝国時代(1868年1945年)に成立したと目されている。ヨーロッパにならった高等教育機関の設置の際、歴史学の分野は国史、東洋史、西洋史の三部門に分けられた。徳川時代(1603年1868年)までは、漢学の中で中国や朝鮮など東北アジアの歴史研究が行われており、これが帝国時代になると近代的大学制度に包含されるときに東洋史に分類された。ここに日本における東洋史の複雑な性格が生まれることになる。";
    jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);
    end = std::chrono::system_clock::now();
    cout << "elapsed_time(ms):" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<endl;
    start=end;

    return 0;
}
