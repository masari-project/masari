# Docker container
## How to build
Inside the masari root directory, execute
```
docker build -t masari -f docker/Dockerfile .
```

## How to use
To run the daemon, use
```
docker run -it masari
```

In order to use the masari wallet cli, you can use
```
docker run -it masari masari-wallet-cli
```

## Docker compose example
This example runs 2 daemons for HA and a masari wallet rpc with the specified keys
```
version: '3.7'

networks:
  net:
    driver: overlay
    driver_opts:
      encrypted: ''
    attachable: true
    
volumes:
  blockchain:
    name: 'blockchain-{{.Task.Slot}}'
    
services:
  masari_daemon:
    image: masari
    command: ["masarid", "--p2p-bind-ip=0.0.0.0", "--p2p-bind-port=38080", "--rpc-bind-ip=0.0.0.0", "--rpc-bind-port=38081", "--non-interactive", "--confirm-external-bind", "--restricted-rpc"]
    volumes:
      - blockchain:/root/.masari
    networks:
      - net
    ports:
     - target: 38080
       published: 38080
       protocol: tcp
       mode: ingress
     - target: 38081
       published: 38081
       protocol: tcp
       mode: ingress
    deploy:
      replicas: 2
      update_config:
        parallelism: 1
        delay: 10s
  wallet:
    image: masari
    command: ["masari-wallet-rpc", "--wallet-file", "funding", "--password", "", "--daemon-host", "masari_daemon_masari_daemon", "--rpc-bind-port", "11182", "--disable-rpc-login", "--rpc-bind-ip=0.0.0.0", "--confirm-external-bind"]
    volumes:
      - ./wallet.keys:/funding.keys
      - ./wallet.cache:/funding.cache
    networks:
      - net
```