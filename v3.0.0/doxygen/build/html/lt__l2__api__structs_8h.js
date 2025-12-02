var lt__l2__api__structs_8h =
[
    [ "lt_l2_get_info_req_t", "lt__l2__api__structs_8h.html#structlt__l2__get__info__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a78cdcb30ca64079af4ea7386dac9a842", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#ace18d3afaa34eb3088e8cf8bd1bd7a71", null ],
      [ "object_id", "lt__l2__api__structs_8h.html#a2401d5d562f8d54312aa24a559164cfa", null ],
      [ "block_index", "lt__l2__api__structs_8h.html#ad64527e659871f86e1415390219a96fd", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a75f5d4b1b766363493433499abcd9ff0", null ]
    ] ],
    [ "lt_l2_get_info_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__get__info__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#a21e40e96b70e995f28048955454522da", null ],
      [ "status", "lt__l2__api__structs_8h.html#a03f45451e913d44722d918fdc581bc0b", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a9cf8f55c268bdf046a43773f44ee450b", null ],
      [ "object", "lt__l2__api__structs_8h.html#ae02312a6c0d056eb47a942d3d38b6e10", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a8e7a739e62d2ce5a53cd2017ef5054af", null ]
    ] ],
    [ "lt_l2_handshake_req_t", "lt__l2__api__structs_8h.html#structlt__l2__handshake__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a4953574605532f7d06c9178f11ecd1b1", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#ae05f758eee766db56bd6d239a7dc8cd6", null ],
      [ "e_hpub", "lt__l2__api__structs_8h.html#a0c9019eb614da823d16b5169fe10adca", null ],
      [ "pkey_index", "lt__l2__api__structs_8h.html#acf5b3cbcf04897534cb31fa6c6239ca4", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a47a4d2560397bc79d0e77c47e1d5e2c2", null ]
    ] ],
    [ "lt_l2_handshake_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__handshake__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#a47df7224a4b4c02ed51b2cf7c0e4087a", null ],
      [ "status", "lt__l2__api__structs_8h.html#a4b7e3a433f87b9964998645466bca364", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#abef09b4babe16e672d4a0e767a0e95b5", null ],
      [ "e_tpub", "lt__l2__api__structs_8h.html#a838f4049cd0dbcc2ac61924c74b0a5d8", null ],
      [ "t_tauth", "lt__l2__api__structs_8h.html#ab525843478ee5804330bf82f7326d3af", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a4ccec4c322bcecb04e6ee3a2091ebc76", null ]
    ] ],
    [ "lt_l2_encrypted_cmd_req_t", "lt__l2__api__structs_8h.html#structlt__l2__encrypted__cmd__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#afdf517520f4c8ac4d1f43dfb7350deaf", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#a321eb1b270d7ca663cf25b274428162c", null ],
      [ "l3_chunk", "lt__l2__api__structs_8h.html#a45de2701a842a091b6ef9d3d7c1c6f22", null ],
      [ "crc", "lt__l2__api__structs_8h.html#ad02d75f9e5baf75543f57d66cfba0ac7", null ]
    ] ],
    [ "lt_l2_encrypted_cmd_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__encrypted__cmd__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#a1862f68972f185ac73d0824da59a2e70", null ],
      [ "status", "lt__l2__api__structs_8h.html#a123f307f87c52deaa8ea352a6ebc6533", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#ae9218f9f079b1f25bb001917f8f50b86", null ],
      [ "l3_chunk", "lt__l2__api__structs_8h.html#a43dcadea599eef725309c2714d139c26", null ],
      [ "crc", "lt__l2__api__structs_8h.html#acc0cae6f76abf9e3ca8186d8d95f36eb", null ]
    ] ],
    [ "lt_l2_encrypted_session_abt_req_t", "lt__l2__api__structs_8h.html#structlt__l2__encrypted__session__abt__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a59839db28b4e3928018f3a122edc1e5e", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#aecbdf1c4e60d354acb6321b55f34245f", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a0791040109c3490e10b1e0c416ba5f64", null ]
    ] ],
    [ "lt_l2_encrypted_session_abt_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__encrypted__session__abt__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#acc74cd36a417019f0e09c3adf8b13e9c", null ],
      [ "status", "lt__l2__api__structs_8h.html#a9e1b132d82376d5e0a809ee4f12ceb6e", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#aec5b3536b621dc21c28f8a920730cab5", null ],
      [ "crc", "lt__l2__api__structs_8h.html#ab9193be472ac4de35e3384a28a4a02f5", null ]
    ] ],
    [ "lt_l2_resend_req_t", "lt__l2__api__structs_8h.html#structlt__l2__resend__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#aa0d8c0964532cfc091a4f9c0a9021888", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#a96e1e9e8f1e724d3594cb9a9060575dc", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a3fb5c17c1d61c36bb9a5791a5f43954a", null ]
    ] ],
    [ "lt_l2_resend_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__resend__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#ab6d8b942a68a4be717c2024d0673cee1", null ],
      [ "status", "lt__l2__api__structs_8h.html#abdba5a82710aa4bc5aea383f9c9e6987", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a6d91c792be2a9e07bd2efab8340107c0", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a5bb2cbd656485cd24710449618ce4f0f", null ]
    ] ],
    [ "lt_l2_sleep_req_t", "lt__l2__api__structs_8h.html#structlt__l2__sleep__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a2cd1dcefc80a9188cc30cbdc32e7b582", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#a26f52b8e5d80ac921acb9cde588f7eba", null ],
      [ "sleep_kind", "lt__l2__api__structs_8h.html#a127ee5482128c48c6f94aea44e62b77e", null ],
      [ "crc", "lt__l2__api__structs_8h.html#addac5e26792659e686eef34e89e19d82", null ]
    ] ],
    [ "lt_l2_sleep_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__sleep__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#aafa1d720478e9976e81aba554f5b2c1c", null ],
      [ "status", "lt__l2__api__structs_8h.html#a5ef473177c835b7e9ccee2940d21f556", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a1b6cb56949f89087bcb2b309bd99286a", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a970e79384a73ec9718de6d03ad8f5173", null ]
    ] ],
    [ "lt_l2_startup_req_t", "lt__l2__api__structs_8h.html#structlt__l2__startup__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#aef6b79d1df887654edab54cb0b7eb835", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#ad48ff09633ddabf6646fb0e49fdf5d05", null ],
      [ "startup_id", "lt__l2__api__structs_8h.html#a4b80f71d3cab58fbc242deb7ec5948c7", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a4fb550eb1df4944cf7f9cb889f8fe4de", null ]
    ] ],
    [ "lt_l2_startup_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__startup__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#afb30cc5ec868c2dd910c67eb4f4be2af", null ],
      [ "status", "lt__l2__api__structs_8h.html#ad5739b4c31b0691d886ce4f3ece3cb52", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a996401a4983764ea7fefb9d4d40063fb", null ],
      [ "crc", "lt__l2__api__structs_8h.html#ac85c56d4611a05cce4be4278ab11b4ce", null ]
    ] ],
    [ "lt_l2_mutable_fw_update_req_t", "lt__l2__api__structs_8h.html#structlt__l2__mutable__fw__update__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a22c9e790ad0d4776b3e0827d7311fc6c", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#a4237454af577e1784d290339a14e681d", null ],
      [ "signature", "lt__l2__api__structs_8h.html#af2d64daec1732fc8ce5770118b5450a6", null ],
      [ "hash", "lt__l2__api__structs_8h.html#ab23bfcf15082092415a128fe77316b6e", null ],
      [ "type", "lt__l2__api__structs_8h.html#a71da21935af7cb79e50fa225b4e7244f", null ],
      [ "padding", "lt__l2__api__structs_8h.html#a39bbeae3c85b5129c0a15f08918c58d5", null ],
      [ "header_version", "lt__l2__api__structs_8h.html#a9157c6cba68d25cb4b2cb71c66ed3e4b", null ],
      [ "version", "lt__l2__api__structs_8h.html#a0e0bc37a781e06ecb6b8ab48e592fbef", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a3ac45ed3b3ca83eb6031d2c9ccb59085", null ]
    ] ],
    [ "lt_l2_mutable_fw_update_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__mutable__fw__update__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#aa95ac5e4eff0b791092a3538269f1afb", null ],
      [ "status", "lt__l2__api__structs_8h.html#ae7ef81822162e2cc40f97488fd3232e1", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a7ae1c7fbabfdc51a6a3cd05c90f13e66", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a31193c60b2ad87632f63787f63d3fca6", null ]
    ] ],
    [ "lt_l2_mutable_fw_update_data_req_t", "lt__l2__api__structs_8h.html#structlt__l2__mutable__fw__update__data__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#aacdcc27999ac3b30aa7f1b433a21487d", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#aaaa869ba3b209129f97954ea340ec635", null ],
      [ "hash", "lt__l2__api__structs_8h.html#aaf7f431a4f20691c4bc787d4040218c6", null ],
      [ "offset", "lt__l2__api__structs_8h.html#a3e024a63db5e44b54a94f0ab37f7eb10", null ],
      [ "data", "lt__l2__api__structs_8h.html#ad774acccba1f90d16dfc1a22b6a600d9", null ],
      [ "crc", "lt__l2__api__structs_8h.html#ab44309a827d2db09b6378f00e4611b1f", null ]
    ] ],
    [ "lt_l2_mutable_fw_update_data_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__mutable__fw__update__data__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#ab321800d88c9931fe464af04a4542819", null ],
      [ "status", "lt__l2__api__structs_8h.html#a62e1824ae946896a81349a7c6efbe21a", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#aa6fea5d11e3f13bab80d0fcdbebbfbb7", null ],
      [ "crc", "lt__l2__api__structs_8h.html#a39cf4042848ceb81c4728afbb1c7cd03", null ]
    ] ],
    [ "lt_l2_mutable_fw_erase_req_t", "lt__l2__api__structs_8h.html#structlt__l2__mutable__fw__erase__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a84f9f2b459ab7f16d2cc62025bc5571c", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#a18d91f715c7581b26ca11760f1ab3517", null ],
      [ "bank_id", "lt__l2__api__structs_8h.html#a477e0d6c8b00f0205452ec0c1ffde662", null ],
      [ "crc", "lt__l2__api__structs_8h.html#ab8dd9944a1c407f7df4fe4dfd0c0f86e", null ]
    ] ],
    [ "lt_l2_mutable_fw_erase_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__mutable__fw__erase__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#a3a95117fb684a89540c6eeed8ab99e28", null ],
      [ "status", "lt__l2__api__structs_8h.html#a5d6bb3c9fb4bd8b64119f326cf1193fe", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a52898a05ab5534712c053030c799f7e1", null ],
      [ "crc", "lt__l2__api__structs_8h.html#ad55534b7c443ee9d23f696d0d027d9cc", null ]
    ] ],
    [ "lt_l2_get_log_req_t", "lt__l2__api__structs_8h.html#structlt__l2__get__log__req__t", [
      [ "req_id", "lt__l2__api__structs_8h.html#a42cfbcddade902ffe3b2212de27fa545", null ],
      [ "req_len", "lt__l2__api__structs_8h.html#a8a640d503a7da6d00ece6ee1ed12d532", null ],
      [ "crc", "lt__l2__api__structs_8h.html#af75446f7815b6bf7b0adca1a9cdaf6ac", null ]
    ] ],
    [ "lt_l2_get_log_rsp_t", "lt__l2__api__structs_8h.html#structlt__l2__get__log__rsp__t", [
      [ "chip_status", "lt__l2__api__structs_8h.html#ac66aeb7dce9ae6cc3f13f97069d4d216", null ],
      [ "status", "lt__l2__api__structs_8h.html#aa3b329d16115cafb7b13202475a06733", null ],
      [ "rsp_len", "lt__l2__api__structs_8h.html#a8076662909e07b3f4c957d1f2539e64d", null ],
      [ "log_msg", "lt__l2__api__structs_8h.html#a87b1a646f1a8367819ba4d81da52c8fa", null ],
      [ "crc", "lt__l2__api__structs_8h.html#adba8ca0420335f840ac4f151bf2643cd", null ]
    ] ],
    [ "TR01_L2_GET_INFO_REQ_ID", "lt__l2__api__structs_8h.html#a8484e04d3d8e66f3d9051611aceb9f7b", null ],
    [ "TR01_L2_GET_INFO_REQ_LEN", "lt__l2__api__structs_8h.html#a3b31d64c0592dfc48d6d40f10d1b0e01", null ],
    [ "TR01_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE", "lt__l2__api__structs_8h.html#a23c0c651967430598687c48afde18353", null ],
    [ "TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127", "lt__l2__api__structs_8h.html#a9ebfee04fb6cd7f0fa9a837381346258", null ],
    [ "TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255", "lt__l2__api__structs_8h.html#a73784c75b80da1af218153d2a67c34c6", null ],
    [ "TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383", "lt__l2__api__structs_8h.html#ab1634fecd417ffff7ecaa25bc71d39b8", null ],
    [ "TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511", "lt__l2__api__structs_8h.html#a68f36fc8948d2b3397e10611615fdc7f", null ],
    [ "TR01_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID", "lt__l2__api__structs_8h.html#ad44bc31f9332332e7f16c8daac934404", null ],
    [ "TR01_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION", "lt__l2__api__structs_8h.html#a9ea1fbcb91121862c2d0a46f59190060", null ],
    [ "TR01_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION", "lt__l2__api__structs_8h.html#aff239a7100d527a0858106c634c1dfe4", null ],
    [ "TR01_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK", "lt__l2__api__structs_8h.html#a1abdf9cdf4052b9e51aac8ff18b1e0a4", null ],
    [ "TR01_L2_GET_INFO_RSP_LEN_MIN", "lt__l2__api__structs_8h.html#ab584c34968c4a208a3f3f2161176e33c", null ],
    [ "TR01_L2_HANDSHAKE_REQ_ID", "lt__l2__api__structs_8h.html#ac09ac7f9551d5c3bae77a3faba67ed33", null ],
    [ "TR01_L2_HANDSHAKE_REQ_LEN", "lt__l2__api__structs_8h.html#a602c48ac9b1bb94c4bcc5657126188c1", null ],
    [ "TR01_L2_HANDSHAKE_RSP_LEN", "lt__l2__api__structs_8h.html#ad519597a93c95cb3b2bf8f2d56f859a1", null ],
    [ "TR01_L2_ENCRYPTED_CMD_REQ_ID", "lt__l2__api__structs_8h.html#a5c80b8f21a062e4d7df637737efa72c8", null ],
    [ "TR01_L2_ENCRYPTED_CMD_REQ_LEN_MIN", "lt__l2__api__structs_8h.html#aa98394e6709a5ea7e27da61922a66961", null ],
    [ "TR01_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MIN", "lt__l2__api__structs_8h_a383468445fe46575b21fce8424b425ce.html#a383468445fe46575b21fce8424b425ce", null ],
    [ "TR01_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MAX", "lt__l2__api__structs_8h_a7993f2e1834ee3660219dcfdf51f1f3d.html#a7993f2e1834ee3660219dcfdf51f1f3d", null ],
    [ "TR01_L2_ENCRYPTED_CMD_RSP_LEN_MIN", "lt__l2__api__structs_8h.html#ac16e72583aeb269ddb20f13693197ab2", null ],
    [ "TR01_L2_ENCRYPTED_SESSION_ABT_ID", "lt__l2__api__structs_8h.html#abf7f9884137cd121118f9708482efc56", null ],
    [ "TR01_L2_ENCRYPTED_SESSION_ABT_LEN", "lt__l2__api__structs_8h.html#ad46bab229c8af1ea90d74df4b84f49a0", null ],
    [ "TR01_L2_ENCRYPTED_SESSION_ABT_RSP_LEN", "lt__l2__api__structs_8h.html#a80820bbd06a549a7109a9a292d59c321", null ],
    [ "TR01_L2_RESEND_REQ_ID", "lt__l2__api__structs_8h.html#a813eb4ebcccac8033d77a05b120521b8", null ],
    [ "TR01_L2_RESEND_REQ_LEN", "lt__l2__api__structs_8h.html#a026cdd814bce48308a57d23fd9df43ab", null ],
    [ "TR01_L2_RESEND_RSP_LEN", "lt__l2__api__structs_8h.html#a1590e37603c94370b5da0ac67a7e4079", null ],
    [ "TR01_L2_SLEEP_REQ_ID", "lt__l2__api__structs_8h.html#a1836bcda718d744639a977c9f1a2ec12", null ],
    [ "TR01_L2_SLEEP_REQ_LEN", "lt__l2__api__structs_8h.html#a134949b53c54ee4f5fc14bfc0d09e032", null ],
    [ "TR01_L2_SLEEP_REQ_SLEEP_KIND_SLEEP_MODE", "lt__l2__api__structs_8h.html#a43ad5929d08c3d9b71e5b8140f508938", null ],
    [ "TR01_L2_SLEEP_RSP_LEN", "lt__l2__api__structs_8h.html#a2bd54d0367a258f6912f01a6b27572f2", null ],
    [ "TR01_L2_STARTUP_REQ_ID", "lt__l2__api__structs_8h.html#af4bd939b1aaf4c7e3b38c4662634ad3d", null ],
    [ "TR01_L2_STARTUP_REQ_LEN", "lt__l2__api__structs_8h.html#acf07ba3cd37b53d3eb8d83afccd613f1", null ],
    [ "TR01_L2_STARTUP_RSP_LEN", "lt__l2__api__structs_8h.html#a6a909d05e2a5d45186e36de2ddc7d2f6", null ],
    [ "TR01_L2_MUTABLE_FW_UPDATE_REQ_ID", "lt__l2__api__structs_8h.html#a4d24988f4b94a6548a2d0d53dfc00b91", null ],
    [ "TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN", "lt__l2__api__structs_8h.html#abe203e39d8acfec63cbe79be2a8d1b65", null ],
    [ "TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN", "lt__l2__api__structs_8h.html#a5768ba298855dca516003f12a67cc0c4", null ],
    [ "TR01_L2_MUTABLE_FW_UPDATE_REQ_TYPE_FW_TYPE_CPU", "lt__l2__api__structs_8h.html#a77da6934295c086e1fcfe9f089da29c9", null ],
    [ "TR01_L2_MUTABLE_FW_UPDATE_REQ_TYPE_FW_TYPE_SPECT", "lt__l2__api__structs_8h.html#a4287a546a9efb529938bba669c3ee015", null ],
    [ "TR01_L2_MUTABLE_FW_UPDATE_DATA_REQ", "lt__l2__api__structs_8h.html#ac54f1c6d841483c9756836ad18c7f7a1", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_REQ_ID", "lt__l2__api__structs_8h.html#a7b0b39185af3bbdc178ea3c8ba3ed19d", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_REQ_LEN", "lt__l2__api__structs_8h.html#a31be3b2628f43b1a34e3c74b7a397f71", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW1", "lt__l2__api__structs_8h.html#a1e023b5f566577f0b69f4da2cd7f6641", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW2", "lt__l2__api__structs_8h.html#abdec3df9e605287656888be20cfb73c1", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT1", "lt__l2__api__structs_8h.html#a3feb36d62c5e82773f8f42f29a774cd9", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT2", "lt__l2__api__structs_8h.html#a54023b7e87a5d49b5a8fa1caac279a7d", null ],
    [ "TR01_L2_MUTABLE_FW_ERASE_RSP_LEN", "lt__l2__api__structs_8h.html#adb99b2327a7c60173e1ff072a733298b", null ],
    [ "TR01_L2_GET_LOG_REQ_ID", "lt__l2__api__structs_8h.html#a591c552c54f742e935c22c03a3f999d8", null ],
    [ "TR01_L2_GET_LOG_REQ_LEN", "lt__l2__api__structs_8h.html#a9d620e79d3198f7fc345bc57f569e54a", null ],
    [ "TR01_L2_GET_LOG_RSP_LEN_MIN", "lt__l2__api__structs_8h.html#a5d009fc768ea8d5979175c505cb2ae0c", null ]
];