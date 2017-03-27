# iroha-ametsuchi

[![Join the chat at https://gitter.im/hyperledger-iroha/Lobby](https://badges.gitter.im/hyperledger-iroha/Lobby.svg)](https://gitter.im/hyperledger-iroha/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[![CircleCI](https://circleci.com/gh/hyperledger/iroha-ametsuchi/tree/master.svg?style=svg)](https://circleci.com/gh/hyperledger/iroha-ametsuchi/tree/master)

[![Travis CI](https://travis-ci.org/hyperledger/iroha-ametsuchi.svg?branch=master)](https://travis-ci.org/hyperledger/iroha-ametsuchi)

Flatbuffer database for the Hyperledger Iroha project.

**あ**らさじと　うちかへすらし　をやまだの　なはしろみづに　ぬれてつくる**あ**

**め**もはるに　ゆきまもあをく　なりにけり　いまこそのべに　わかなつみて**め**

**つ**くばやま　さけるさくらの　にほひをぞ　いりてをらねど　よそながらみ**つ**

**ち**ぐさにも　ほころぶはなの　しげきかな　いづらあをやぎ　ぬひしいとす**ぢ**

## Build

Download [FlatBuffers](https://github.com/google/flatbuffers), build it and place the `flatc` executable into your PATH.

Run from the project root:

```sh
cmake -H. -Bbuild
cmake --build build --target genfbs
cmake --build build
```
