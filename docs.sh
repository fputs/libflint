#!/usr/bin/env bash

mkdocs build
ssh debian@fputs.com rm -rf /var/www/fputs.com/docs/libflint
ssh debian@fputs.com mkdir -p /var/www/fputs.com/docs/libflint
scp -r site/* debian@fputs.com:/var/www/fputs.com/docs/libflint/
