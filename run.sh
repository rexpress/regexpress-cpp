arg=();for var in "$@";do arg+=($(echo -n "$var" | base64 -d)); done; node /root/JavascriptTester.js "${arg[@]}"
