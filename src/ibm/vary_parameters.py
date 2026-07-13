#!/usr/bin/env python3

#!/usr/bin/env python3
import datetime as dt

base_name = "sim_cue_migration_"
current_time = dt.datetime.now()
base_name += current_time.strftime("%Y%m%d_%H%M%S")

n_sites = 10

max_generation = 100
max_season_time_steps = 50

mu_n = [0.02]
mu_x = [0.02]
mu_t = [0.02]
mu_nu = [0.02]
mu_xo = [0.02]

mu_ap = 0
mu_bp = 0



nrep = 3
ctr = 1

pr_base_flight_survive = 0.1
flight_survive_scale = 0.7
flight_survive_power = 1.0 

f = [1.0]
g = [3.0]


exe = "./migration_cues.exe"

for i in range(0,nrep):
    for mu_n_i in mu_n:

        mu_an = mu_n_i
        mu_bn = mu_n_i

        for mu_x_i in mu_x:
            mu_ax = mu_x_i
            mu_bx = mu_x_i

            for mu_t_i in mu_t:
                mu_at = mu_t_i
                mu_bt = mu_t_i

                for mu_nu_i in mu_nu:
                    mu_anu = mu_nu_i
                    mu_bnu = mu_nu_i

                    for mu_xo_i in mu_xo:
                        mu_axo = mu_xo_i
                        mu_bxo = mu_xo_i

                        for f_i in f:
                            for g_i in g:

                                file_name = f"{base_name}_{ctr}"

                                ctr +=1

                                job_str = f"{exe} " +\
                                            f"{file_name} " +\
                                            f"{max_generation} " +\
                                            f"{max_season_time_steps} " +\
                                            f"{mu_an} " +\
                                            f"{mu_bn} " +\
                                            f"{mu_ax} " +\
                                            f"{mu_bx} " +\
                                            f"{mu_at} " +\
                                            f"{mu_bt} " +\
                                            f"{mu_ap} " +\
                                            f"{mu_bp} " +\
                                            f"{mu_anu} " +\
                                            f"{mu_bnu} " +\
                                            f"{mu_axo} " +\
                                            f"{mu_bxo} " +\
                                            f"{pr_base_flight_survive} " +\
                                            f"{flight_survive_scale} " +\
                                            f"{flight_survive_power} " +\
                                            f"{f_i} " +\
                                            f"{g_i} "

                                print(job_str)
