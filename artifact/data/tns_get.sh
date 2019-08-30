#!/bin/bash
wget https://s3.us-east-2.amazonaws.com/frostt/frostt_data/chicago-crime/geo/chicago-crime-geo.tns.gz
gunzip chicago-crime-geo.tns.gz
wget https://s3.us-east-2.amazonaws.com/frostt/frostt_data/nips/nips.tns.gz
gunzip nips.tns.gz
wget https://s3.us-east-2.amazonaws.com/frostt/frostt_data/enron/enron.tns.gz
gunzip enron.tns.gz
wget https://s3.us-east-2.amazonaws.com/frostt/frostt_data/nell/nell-2.tns.gz
gunzip nell-2.tns.gz
