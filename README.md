# Ad hoc converter for JSON to SQL

## Purpose

**mtjson2sql** converts large JSON report to SQL file you can load directly to database. It's based on [pdjson](https://github.com/skeeto/pdjson) (for stream parsing) and [inih](https://github.com/benhoyt/inih) (for config handling) libraries.

Official reports about stolen and lost mobile phones in Ukraine are provided by [data.gov.ua](https://data.gov.ua).
You can [download report](https://data.gov.ua/dataset/30b67898-1968-4d99-8058-298b56f22bff) manually or via API (URL of report page can change over time).

## Get started

- git clone this repo
- cd to directory
- run make build
- edit config.ini for your needs
- run mtjson2sql


