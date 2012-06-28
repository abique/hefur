#! /bin/bash

LESSC=$HOME/.npm/bin/lessc
#UGLIFYJS=$HOME/.npm/bin/uglifyjs

cp hefur.less bootstrap/less/ &&
cd bootstrap/less &&
$LESSC hefur.less >../../css/hefur.css &&
cd ../..

# wget http://code.jquery.com/jquery-1.7.2.min.js -O js/jquery.min.js
