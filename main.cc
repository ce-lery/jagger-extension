#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <chrono>  
#include <jagger.h>

using namespace std;

int main(void) 
{
    // read jagger model
    string model ("../jagger-2023-02-18/model/kwdlc/patterns");
    Jagger jagger_parser;
    jagger_parser.read_model(model);

    vector<string> sentences = {
        "おまえは今まで食ったパンの枚数をおぼえているのか？",
        "この味は！ ………ウソをついている『味』だぜ……",
        "あ…ありのまま 今起こった事を話すぜ！",
        // "日本における東洋史の概念は、帝国時代(1868年1945年)に成立したと目されている。ヨーロッパにならった高等教育機関の設置の際、歴史学の分野は国史、東洋史、西洋史の三部門に分けられた。徳川時代(1603年1868年)までは、漢学の中で中国や朝鮮など東北アジアの歴史研究が行われており、これが帝国時代になると近代的大学制度に包含されるときに東洋史に分類された。ここに日本における東洋史の複雑な性格が生まれることになる。",
        //"私は香川出身で、超有名で、約5年です。",
        //"1999年6月のことです。",
        //"1999"
    };

    chrono::system_clock::time_point start, end;
    uint32_t elapsed_time=0;
    start = chrono::system_clock::now();

    for(auto sentence:sentences){
        vector<string> morpheme,morpheme_form;
        jagger_parser.DivideMorpheme(sentence,morpheme,morpheme_form);
        end = std::chrono::system_clock::now();
        cout << "elapsed_time(us):" << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<<endl;
        start=end;

        if((int)morpheme.size()!=(int)morpheme_form.size()){
            cout <<"size not match" << endl;
            continue;
        }
        for(int i=0;i<(int)morpheme.size();i++) cout << morpheme[i] <<"\t\t"<< morpheme_form[i] << endl;
        // cout <<"morphe size:" << (int)morpheme.size()<<endl;
        // cout <<"morpheme_form size:" << (int)morpheme_form.size()<<endl;

        // for(auto morpheme_part:morpheme) cout << morpheme_part << endl;
        // for(auto morpheme_form_part:morpheme_form) cout << morpheme_form_part << endl;
    }

    return 0;
}
