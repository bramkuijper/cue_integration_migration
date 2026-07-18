#!/usr/bin/env bash

find . -iregex ".*sim_cue_migration.*[0-9]" -print0 | xargs -0 -P4 -I% ./plot_output.r %
find . -iregex ".*sim_cue_migration.*migration" -print0 | xargs -0 -P4 -I% ./plot_migration.r %

rm -rf Rplots.pdf
