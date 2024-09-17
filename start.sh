cd /applications/fileServer
pm2 start fileserver.js

cd /applications/DFA/Server
pm2 start DFAserver.js

cd /applications/LL1
pm2 start ll1server.js

docker start opengauss
cd /applications/WWDLDG/ahu_-second-goods
pm2 start wwdldg_server.js

nginx