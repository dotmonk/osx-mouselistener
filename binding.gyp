 {
    "targets": [
      {
        "target_name": "osx-mouselistener",
        "conditions": [
            ["OS=='mac'", {
              'LDFLAGS': [
                '-framework IOKit',
                '-framework CoreFoundation'
              ],
              'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'OTHER_LDFLAGS': [
                  '-framework IOKit',
                  '-framework CoreFoundation'
                ],
              },
              "sources": [ "osx-mouselistener.cc" ]
            }]
          ],
        "include_dirs" : [
          "<!(node -e \"require('nan')\")"
        ]
      }
    ]
  }
