#!/bin/bash

# set log
mkdir -p results/log/$(basename "$0" .sh)
log=results/log/$(basename "$0" .sh)/$(date +%Y%m%d_%H%M%S).log
exec &> >(tee -a $log)
set -x

#!/bin/bash
wget http://www.tkl.iis.u-tokyo.ac.jp/~ynaga/jagger/jagger-2023-02-18.tar.gz
tar zxvf jagger-2023-02-18.tar.gz
cd jagger-2023-02-18

#  1) prepare a dictionary in the format compatible with mecab-jumandic (cf. mecab-jumandic-7.0-20130310.tar.gz)
wget https://www.mirrorservice.org/sites/distfiles.macports.org/mecab/mecab-jumandic-7.0-20130310.tar.gz  -O mecab-jumandic-7.0-20130310.tar.gz
tar zxfv mecab-jumandic-7.0-20130310.tar.gz
# mv mecab-jumandic-7.0-20130310 mecab-jumandic
# cd mecab-jumandic-7.0-20130310
wget "http://www.tkl.iis.u-tokyo.ac.jp/~ynaga/jagger/mecab-jumandic-7.0-20130310.patch"
patch -p0 < mecab-jumandic-7.0-20130310.patch 

# 2) Use the Kyoto University Web Document Leads Corpus (default)
git clone https://github.com/ku-nlp/KWDLC
./configure

# 3) Train a model from the standard split, evaluate the resulting model, and then install
make model-benchmark && make install

# 4) cp jagger-extension
cd ../
rm jagger-2023-02-18/src/jagger.cc
rm jagger-2023-02-18/src/ccedar_core.h

cp jagger-extension/jagger.cc jagger-2023-02-18/src/
cp jagger-extension/ccedar_core.h jagger-2023-02-18/src/

# 5) remove unneccesary file
rm jagger-2023-02-18.tar.gz

set +x