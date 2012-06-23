#! /bin/bash

LESSC=$HOME/.npm/bin/lessc

cp hefur.less bootstrap/less/ &&
cd bootstrap/less &&
$LESSC hefur.less >../../css/hefur.css
