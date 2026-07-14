library("tidyverse")

file_name <- commandArgs(trailingOnly = T)[1]

file_name <- "sim_cue_migration_20260714_132649_3_migration"
data <- read.table(file = file_name,
                   sep=";",
                   header=T)

ggplot(data = data %>% filter(generation == max(generation)),
       mapping = aes(x = site, y = ecological_time)) +
    geom_tile(mapping = aes(fill= n)) +
    facet_grid(~ sex)