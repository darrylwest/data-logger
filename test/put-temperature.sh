#!/usr/bin/env bash
# dpw@MacBookPro.localdomain
# 2025-02-15 17:09:16
#

set -eu

curl -H 'Content-Type: application/json' -XPUT -d '{"key":"1739636749.test-location", "value":10.1234}' http://localhost:9090/temperature


