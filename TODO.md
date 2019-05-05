# ECXX

## TODO

- [x] Components. empty_value components data optimization (not stored)
- [x] View. Chose the smallest entity_vector to iterate (less iterations)
- [ ] View. Unify RT/CT view base
- [ ] View. Exclude?
- [ ] View. Reverse sorted indirection (normalize indices back when unpack components) 
- [ ] View. Replace vector to array on RT
- [ ] Groups (aka Managed Family)
- [ ] Custom component data managers

## Design Notes

- Q: entt has back-to-front iteration through component data?

hypo: forward data iteration - better cache locality
is it worth to keep forward data iteration? 
Or read-only/mutable iterations could be provided

## Badges

[![GitHub version](https://badge.fury.io/gh/eliasku%2Fecxx.svg)](http://badge.fury.io/gh/eliasku%2Fecxx)
[![Coverage Status](https://coveralls.io/repos/github/eliasku/ecxx/badge.svg?branch=master)](https://coveralls.io/github/eliasku/ecxx?branch=develop)

## CI

- [ ] travis-ci
- [ ] appveyor
- [ ] coverall

## Docs

- [ ] description
- [ ] goals

## Demo

- web versions are required
- [ ] particles simulation
- [ ] asteroids example
