#!/usr/bin/env Rscript

library("tidyverse")
library("patchwork")

file_name <- commandArgs(trailingOnly = T)[1]

file_name_out <- paste0("graph_",basename(file_name),".pdf")

#file_name <- "sim_cue_migration_20260714_132649_3_migration"
data <- read.table(file = file_name,
                   sep=";",
                   header=T) %>% mutate(
                   n = sqrt(n)
                   )

p1 <- ggplot(data = data %>% filter(generation == max(generation)),
       mapping = aes(x = site, y = ecological_time)) +
    geom_tile(mapping = aes(fill= n)) +
    facet_grid(~ sex) +
    scale_fill_distiller(palette = "Spectral") +
    theme_classic()

p2 <- ggplot(data = data %>% filter(generation == max(generation)),
       mapping = aes(x = site, y = ecological_time)) +
    geom_tile(mapping = aes(fill= resources)) +
    facet_grid(~ sex) +
    scale_fill_distiller(palette = "Spectral") +
    theme_classic()

p3 <- ggplot(data = data %>% filter(generation == max(generation)),
       mapping = aes(x = site, y = ecological_time)) +
    geom_tile(mapping = aes(fill= n_dead)) +
    facet_grid(~ sex) +
    scale_fill_distiller(palette = "Spectral") +
    theme_classic()


p4 <- ggplot(data = data %>% filter(generation == max(generation)),
       mapping = aes(x = site, y = ecological_time)) +
    geom_tile(mapping = aes(fill= av_flight_group_size)) +
    facet_grid(~ sex) +
    scale_fill_distiller(palette = "Spectral") +
    theme_classic()

p5 <- ggplot(data = data %>% filter(generation == max(generation)),
       mapping = aes(x = site, y = ecological_time)) +
    geom_tile(mapping = aes(fill= sqrt(av_pr_fly))) +
    facet_grid(~ sex) +
    scale_fill_distiller(palette = "Spectral") +
    theme_classic()

(p1 / p2 / p3 / p4 / p5)

ggsave(filename = file_name_out, height = 12)
