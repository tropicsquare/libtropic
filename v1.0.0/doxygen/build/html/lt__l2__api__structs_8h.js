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
      [ "log_msg", "lt__l2__api__structs_8h.html#ae64062b773853f69fca33b615d816d0b", null ],
      [ "crc", "lt__l2__api__structs_8h.html#adba8ca0420335f840ac4f151bf2643cd", null ]
    ] ],
    [ "LT_L2_GET_INFO_REQ_ID", "lt__l2__api__structs_8h.html#acabe218e137e76c1f413cf21758ba661", null ],
    [ "LT_L2_GET_INFO_REQ_LEN", "lt__l2__api__structs_8h.html#a016369903f675514bfe3a1f8aae1d8e6", null ],
    [ "LT_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE", "lt__l2__api__structs_8h.html#ada58fcd6b0bdc695f5ccc3bd9a777edb", null ],
    [ "LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127", "lt__l2__api__structs_8h.html#ada8da1721fd1d64df327c935c6d0718f", null ],
    [ "LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255", "lt__l2__api__structs_8h.html#a5c3ed8274fc0abb95360963bab2ce757", null ],
    [ "LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383", "lt__l2__api__structs_8h.html#ac328bf3139f21016e61fba0ad2cd61b2", null ],
    [ "LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511", "lt__l2__api__structs_8h.html#ac696d2f5e11696fd134823296f6e927d", null ],
    [ "LT_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID", "lt__l2__api__structs_8h.html#ab2ed01b6ec98d95831b94658c579b4df", null ],
    [ "LT_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION", "lt__l2__api__structs_8h.html#a126922e97dae8bf1c999a7a360a8e9d8", null ],
    [ "LT_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION", "lt__l2__api__structs_8h.html#a40da202821e0aa7fd07a3d43c6f25f0e", null ],
    [ "LT_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK", "lt__l2__api__structs_8h.html#ab7f3743a16f71f2cef247441f3743874", null ],
    [ "LT_L2_GET_INFO_RSP_LEN_MIN", "lt__l2__api__structs_8h.html#afe567152ede3e6b3c7f00befd105e111", null ],
    [ "LT_L2_HANDSHAKE_REQ_ID", "lt__l2__api__structs_8h.html#ab50bbea75ace9ffc69c65e7a982c5d4d", null ],
    [ "LT_L2_HANDSHAKE_REQ_LEN", "lt__l2__api__structs_8h.html#a01566352b0a908024e58408dce8a9579", null ],
    [ "LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_0", "lt__l2__api__structs_8h.html#a27a40ddd8193e3242e643f4c2854eb85", null ],
    [ "LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_1", "lt__l2__api__structs_8h.html#a787e5c2b46604ae6a9d831ed3de0b629", null ],
    [ "LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_2", "lt__l2__api__structs_8h.html#ab8aa0694f0754df60deb195c8151cf86", null ],
    [ "LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_3", "lt__l2__api__structs_8h.html#ae72e815e530d59748efecc8e76c77598", null ],
    [ "LT_L2_HANDSHAKE_RSP_LEN", "lt__l2__api__structs_8h.html#a2e9c9536259ac87ecc27942eb276ab49", null ],
    [ "LT_L2_ENCRYPTED_CMD_REQ_ID", "lt__l2__api__structs_8h.html#a0dc7c6219b897325fd2ec4c743cb8680", null ],
    [ "LT_L2_ENCRYPTED_CMD_REQ_LEN_MIN", "lt__l2__api__structs_8h.html#a413d37bb0b04d047fecfebaf211d768b", null ],
    [ "LT_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MIN", "lt__l2__api__structs_8h_abff57b7229a8c2027e8abd0b65425af9.html#abff57b7229a8c2027e8abd0b65425af9", null ],
    [ "LT_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MAX", "lt__l2__api__structs_8h_a44c877bbeca59143217f17d461b466a0.html#a44c877bbeca59143217f17d461b466a0", null ],
    [ "LT_L2_ENCRYPTED_CMD_RSP_LEN_MIN", "lt__l2__api__structs_8h.html#a117b3a0dba890def65a0df183b8148eb", null ],
    [ "LT_L2_ENCRYPTED_SESSION_ABT_ID", "lt__l2__api__structs_8h.html#a3ded3ba6538bd64c4cd5416eaad016b7", null ],
    [ "LT_L2_ENCRYPTED_SESSION_ABT_LEN", "lt__l2__api__structs_8h.html#a7af725022dc720d10c61b8de8a3ca51c", null ],
    [ "LT_L2_ENCRYPTED_SESSION_ABT_RSP_LEN", "lt__l2__api__structs_8h.html#a637e9e8f5de64ffd8aff4486720063c8", null ],
    [ "LT_L2_RESEND_REQ_ID", "lt__l2__api__structs_8h.html#ae27e06c1927d3080a26807412272fdb2", null ],
    [ "LT_L2_RESEND_REQ_LEN", "lt__l2__api__structs_8h.html#a413019212fa3774721f315bf34e47496", null ],
    [ "LT_L2_RESEND_RSP_LEN", "lt__l2__api__structs_8h.html#ad8bd9ae862b3b3b1148d14e93c54dd00", null ],
    [ "LT_L2_SLEEP_REQ_ID", "lt__l2__api__structs_8h.html#a689d04986c06069a04429c10e614ff1e", null ],
    [ "LT_L2_SLEEP_REQ_LEN", "lt__l2__api__structs_8h.html#a4e9e68cf7cb300f6f260d68ad70726ef", null ],
    [ "LT_L2_SLEEP_REQ_SLEEP_KIND_SLEEP_MODE", "lt__l2__api__structs_8h.html#ae831d678114cfa8884d576a7cd7dd576", null ],
    [ "LT_L2_SLEEP_REQ_SLEEP_KIND_DEEP_SLEEP_MODE", "lt__l2__api__structs_8h.html#a848d4033652cbb1416c726c0b900ad4e", null ],
    [ "LT_L2_SLEEP_RSP_LEN", "lt__l2__api__structs_8h.html#a0bd6ae9d793b6448f4978820c2ec653c", null ],
    [ "LT_L2_STARTUP_REQ_ID", "lt__l2__api__structs_8h.html#ad19f831a792331cb0081fb01aeb2b561", null ],
    [ "LT_L2_STARTUP_REQ_LEN", "lt__l2__api__structs_8h.html#a219cb5130412d3426eb48f620e1d6f56", null ],
    [ "LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT", "lt__l2__api__structs_8h.html#a5435b4af1b3a7d5181ba11c0e5385c9c", null ],
    [ "LT_L2_STARTUP_REQ_STARTUP_ID_MAINTENANCE_REBOOT", "lt__l2__api__structs_8h.html#a2bc0022e0aeeedb9d1d21b0ef0dd23eb", null ],
    [ "LT_L2_STARTUP_RSP_LEN", "lt__l2__api__structs_8h.html#a7c7c5c0e89a64b6d2760c970b754f8ba", null ],
    [ "LT_L2_MUTABLE_FW_UPDATE_REQ_ID", "lt__l2__api__structs_8h.html#a48e2c44bb351a56b3160eedb59d7a7a7", null ],
    [ "LT_L2_MUTABLE_FW_UPDATE_REQ_LEN", "lt__l2__api__structs_8h.html#a41105b015b11b867c2217dc7ea5ba81f", null ],
    [ "LT_L2_MUTABLE_FW_UPDATE_RSP_LEN", "lt__l2__api__structs_8h.html#a95c67407910052179c490b416dc245e3", null ],
    [ "LT_L2_MUTABLE_FW_UPDATE_REQ_TYPE_FW_TYPE_CPU", "lt__l2__api__structs_8h.html#accfa9e7be122707aa0e5027ae2baef7d", null ],
    [ "LT_L2_MUTABLE_FW_UPDATE_REQ_TYPE_FW_TYPE_SPECT", "lt__l2__api__structs_8h.html#aafdc6cdf13677bdde2041649cfd0a6e3", null ],
    [ "TS_L2_MUTABLE_FW_UPDATE_DATA_REQ", "lt__l2__api__structs_8h.html#a1c74c70111a2acc5a6847c4b8a72b868", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_REQ_ID", "lt__l2__api__structs_8h.html#a1d9fb9ebb458b322fff7a41e0690fea8", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_REQ_LEN", "lt__l2__api__structs_8h.html#a0e748b89cf2f1beb631203916662c2ec", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW1", "lt__l2__api__structs_8h.html#a99eb726ff2fe0db7784a165192cbde69", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW2", "lt__l2__api__structs_8h.html#a72f63ed5101c0f5995fd298ffe1e871e", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT1", "lt__l2__api__structs_8h.html#a6f98191bd84cda31dc9d9cd15d733976", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT2", "lt__l2__api__structs_8h.html#aa69c86fbc2c3ede86177f3be769ff49a", null ],
    [ "LT_L2_MUTABLE_FW_ERASE_RSP_LEN", "lt__l2__api__structs_8h.html#a2732f3db1540fe66a3cbf27cc5e86fcb", null ],
    [ "LT_L2_GET_LOG_REQ_ID", "lt__l2__api__structs_8h.html#a928169386147534adab10a2942d3706c", null ],
    [ "LT_L2_GET_LOG_REQ_LEN", "lt__l2__api__structs_8h.html#ab32f9c5e75bba4718bf50efac6d8f313", null ],
    [ "LT_L2_GET_LOG_RSP_LEN_MIN", "lt__l2__api__structs_8h.html#ace2358e053f596b9a6453580ff6eaa74", null ]
];