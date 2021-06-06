cleos -u https://eos.greymass.com push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "sellram",
      "data": {
        "account": "nxwrm3fwgz3q",
        "bytes": "89"
      },
      "authorization": [
        {
          "actor": "nxwrm3fwgz3q",
          "permission": "owner"
        }
      ]
    }
  ]
}'
