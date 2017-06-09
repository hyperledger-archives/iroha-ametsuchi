#!/bin/bash -x

docker-compose -f docker-compose-test.yml down 1>/dev/null 2>&1 || true
docker-compose -f docker-compose-test.yml up -d

export PGHOST=192.168.55.2
export PGUSER=zekret
export PGPASSWORD=secretpasswd
export REDISHOST=192.168.55.3

echo "Wait 30 sec.." && sleep 30

cmake -Bbuild -H..
cd build && make -j10 && ctest
