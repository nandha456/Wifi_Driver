savedcmd_wifi-cfg.mod := printf '%s\n'   wifi-cfg.o | awk '!x[$$0]++ { print("./"$$0) }' > wifi-cfg.mod
