# 修正結果（2026-10-05）

結果: **fixed（macOS/POSIX上で検証済み）**。監査で報告した13件に修正と回帰検証を追加しました。以下に元の監査レポートをそのまま保存しています。元レポートの行番号、severity、静的検証に関する記述は、修正前の revision `138e5f822aefff6bf39bb88a89e80be76228a984` を指します。

PRブランチは最新masterの `8fbf1197fc8f75dea0272ed376f8fbbf27a3ca99` に統合しました。この更新に含まれるHTTP EOF/受信エラー修正とESP-IDFの既存ライフサイクルを保持し、追加の長さ検証・失敗時の内容消去・回帰テストを統合しています。

## 修正と検証の対応

| 元レポート | 修正した境界 | 検証 |
|---|---|---|
| 1, 2 HTTP受信エラー・EOF | 符号付き受信結果を検査してからオフセットを更新。切断・エラー・長さの不正値を失敗として返し、要求・応答・クライアント送信まで伝播。失敗応答の部分内容を消去 | 固定長・chunkedの部分EOFとEAGAIN、不正長さ、正常な固定長・chunked・接続終了フレーミング、XMLを含む未完了応答の拒否 |
| 3 Objective-Cキャッシュ寿命 | 制御点のロック中に独立したデバイス記述・SCPD・SSDP情報・現在の状態値をコピー。サービス、アクション、状態変数、アイコンが所有元ラッパーを保持 | ASan/UBSanでキャッシュと親ラッパーを破棄してから子ラッパーを参照。名称・状態値・ネイティブリスナーを確認 |
| 4 POSIXスレッド寿命 | runnableと実行終了を分離し、終了通知まで外部停止を待機。自己削除はaction終了後。HTTPクライアントの所有権移譲と排出を同期し、リスナー内のサーバー削除を遅延 | 1秒を超える終了処理、即時停止、再起動、自己削除、HTTPリスナーからのサーバー削除、既存HTTP・UPnPテスト |
| 5 libxml2長さ | 指定されたバイト数をコピーし、int上限と内部NULを検査。UTF-8回復処理の範囲を排他的に制限 | 非NUL終端バッファ、内部NUL、不完全UTF-8、過大な長さ、末尾NULの互換性。libxml2を明示的にリンクしてASan/UBSan検証 |
| 6 TLS検証 | デフォルト信頼ストア・証明書チェーン・DNS名/IP識別子を検証し、DNS SNIを設定。失敗時にTLS状態を解放 | ローカルTLSサーバーで未信頼証明書の拒否、信頼されたlocalhostの成功、IP識別子不一致の拒否 |
| 7 Objective-Cアクションコールバック | ラッパー作成時にraw selfをネイティブリスナーへ登録しない。既存ネイティブリスナーを保持し、ホスト側は公開されたdevice delegateで配送 | ラッパー作成・解放後にも元のネイティブリスナーとuserdataが保持され、呼び出せることを確認 |
| 8 XML深さ | 両バックエンドでノード作成前に128階層の制限を適用し、エラー後の再帰的解放も制限 | 許容境界、境界超過、10,000階層の未完XML、通常の兄弟ノードをExpat/libxml2で検証 |
| 9 SSDP MX | 数字を逐次検査してオーバーフローを避け、正のMXを5秒以内へ制限してから待ち時間を計算 | 0、通常値、6、INT_MAX、非常に長い数値、符号・文字混入 |
| 10 テスト用presentation | 未初期化スタック配列を固定HTMLに置換 | ローカルHTTP要求で応答内容の完全一致を確認 |
| 11 メディア階層循環 | 各走査のコピー済みvisited ID集合、深さ64、要求数1024の制限をC/Objective-C/旧サンプルへ適用 | 実際のCサンプルを取り込むfixtureで自己循環・2ノード循環・正常な有限階層・深さ上限をASan/UBSan検証 |
| 12 SSDP tokenizer | トークンのない入力でも確保済みtokenizerを破棄 | 不正入力1,000回の後に正常ヘッダーを処理。解放経路をソース確認 |
| 13 空UDP | recvfromの負のエラーとゼロ長の成功を区別し、両SSDP受信ワーカーは空データグラムを破棄して続行 | 空UDP直後の正常データグラムを実際のSSDP応答ワーカーで処理 |

## 検証コマンドと結果

- `./configure --enable-test --enable-examples CPPFLAGS=-I/opt/homebrew/opt/boost/include LDFLAGS=-L/opt/homebrew/opt/boost/lib`: 成功。
- `make -j4`: 成功。既存Autotools生成ファイルの時刻を整えた後、通常のmakeを利用。
- `make check`: 成功。最終カバレッジ実行内でも**38テストケース**を実行し、失敗なし。
- `./clang-format`: 変更したC/C++/Objective-Cファイルを一時コピーに集めて実行し、その結果だけを反映。既存の未追跡ファイルや無関係なソースは対象外。使用したformatterはXcode CommandLineToolsのclang-format。
- `python3 test/security/verify-macos.py /tmp/mupnp-security-build`: 成功。ASan/UBSan、全38ケース、Objective-C寿命、TLS、メディア循環、libxml2の3ケース、SOCKET_DEBUGの2ケースを検証。
- `git diff --check`、Python検証スクリプトの構文コンパイル: 成功。
- `sh test/espidf/host/check_generators.sh`: 成功。`-DESP_PLATFORM`と既存host shimsで、変更したスレッド・HTTP・ソケット・SSDPソースの構文検証も成功。Linux向けhostテストの実行とESP32実機ビルドは未実施。
- HTTP Content-Lengthの通常の空白・タブを正常対照に加え、ASan/UBSanとSOCKET_DEBUGで再検証: 成功。

Clangとlcovを使った必須カバレッジ検証も成功しました（プロジェクトの行66.9%、関数78.1%）。macOSでは`-lgcov`をアーカイブへ追加できないため、`CODE_COVERAGE_LIBS=`とリンクの`--coverage`でClangランタイムを使用しました。lcov 1.16は一時ディレクトリに取得し、gcovラッパーは`llvm-cov gcov "$@"`を実行しました。

```sh
PATH=/tmp:/tmp/mupnp-lcov/bin:$PATH ./configure \
  --enable-test --enable-examples --enable-code-coverage \
  --with-gcov=mupnp-gcov \
  CPPFLAGS=-I/opt/homebrew/opt/boost/include \
  LDFLAGS=-L/opt/homebrew/opt/boost/lib

PATH=/tmp:/tmp/mupnp-lcov/bin:$PATH make -j4 check-code-coverage \
  CODE_COVERAGE_LIBS= \
  LDFLAGS='-L/opt/homebrew/opt/boost/lib --coverage' \
  CODE_COVERAGE_LCOV_OPTIONS='--gcov-tool /tmp/mupnp-gcov --base-directory /Users/skonno/Src/mupnp/test/unix' \
  CODE_COVERAGE_LCOV_RMOPTS= \
  CODE_COVERAGE_IGNORE_PATTERN="'/Applications/*' '/opt/*'"
```

修正前のHTTP実装だけを差し替えた同じfixtureでは、部分本文受信後のEOFが4秒のタイムアウトまで終了しませんでした。修正後はEOF/EAGAINを失敗として即座に返し、正常な固定長・chunked・接続終了本文は従来どおり取得できました。深いXML、キャッシュ破棄後の子参照、循環するメディア階層、TLSの正常・異常の各対照も上記fixtureで検証しています。

独立した修正前調査と、1回の候補レビューを実施しました。レビューで確認した受信バッファの余分な終端領域、クライアント応答エラー伝播、HTTPリスナー内削除の問題を修正し、対応するテストを追加しています。

## PRのCI指摘への対応

PR #28の初回CIではLinuxビルド、ESP-IDFビルド、host-regressionが成功しました。SonarCloudがSOCKET_DEBUGで1バイトの読み捨て先に終端NULを書き込む既存の境界問題を検出したため、受信バッファを書き換えない長さ指定ログへ変更しました。長いHTTPヘッダーを正常対照に追加し、SOCKET_DEBUGとASan/UBSanで検証しました。メディアサンプル3箇所は `examples/common/content_directory.h` の単一の走査・予算処理を利用し、循環防止の重複と複雑度を解消しています。共通ヘッダーはAutotoolsの配布対象にも追加しました。更新後の38ケース、カバレッジ、サニタイザーfixtureは成功しています。

## 互換性と検証範囲

- 制御点が返すObjective-Cデバイスは独立したスナップショットです。取得済みスナップショットへの後続キャッシュイベントの自動反映はありません。最新情報は再取得してください。サービスURL、SCPD、状態値、子デバイスを保持し、アクション送信は既存のネイティブAPIを利用します。
- POSIXのstopはaction終了まで待ちます。actionの必要とするロックを保持して外部stopを呼ばないでください。HTTPサーバーの自己削除は検証していますが、アプリケーションが任意の所有デバイスを実行中のコールバックから破棄する一般的な寿命管理まで保証する変更ではありません。
- TLSはOpenSSL 1.0.2以上の識別子検証APIを使用します。それ以前のTLSバックエンドは検証できないため接続を拒否します。
- iOS、Windows、組込みOS、歴史的Xcode/CyberLinkプロジェクトは実行していません。macOSのObjective-Cラッパー、ネイティブPOSIX、明示的なOpenSSL/libxml2変種を検証しています。Cメディアサンプルの同等の制限を旧C/Objective-Cサンプルにも適用しましたが、その旧ビルドの実行は未検証です。
- macOSのASan/UBSanを使用し、LeakSanitizerの計測は実施していません。tokenizerの解放は早期returnのソース確認と反復・正常対照テストで確認しました。
- 既存のバージョン定義の重複や非推奨APIに関する警告は残っています。監査の未確認候補や未監査領域は、下記の元レポートに記載された範囲を維持します。

---


# Security Review: mupnp

## Scope

Single-pass static security review of current mUPnP project: native C library and public headers, Objective-C wrappers, runnable examples, tests and legacy AV executable source bodies.

- Scan mode: repository
- Target kind: git_worktree
- Target ID: target_sha256_ac520384bebbff49113ae7d3a8e93534ea782b04f5e6b842337b312b208f3cde
- Revision: 138e5f822aefff6bf39bb88a89e80be76228a984
- Snapshot digest: codex-security-snapshot/v1:sha256:d25a037685a81bb2e3c05f3fa3f7e217e5725f470a6e8c54c27964e9f33ed061
- Inventory strategy: repository
- Included paths: .
- Excluded paths: none
- Scan context: Check the project

Limitations and exclusions:
- No builds, runtime PoCs, test execution, live services, network access, Git history or external vulnerability databases were used.
- 229 implementation/API files fully reviewed; 10 additional files had executable-body review with fixed XML/HTML literals excluded.
- Coverage is partial for historical project configurations, ancillary declarations, fixed literal sections and generated/binary artifacts; a legacy YouTube HTTP candidate remains deferred pending build/reachability evidence.
- One parser investigator final response was blocked by the security access filter. Earlier observations were independently validated from source; its source-coverage-only response was available.
- Excluded doc/\*\*, build/\*\*, generated documentation, object/dependency files, bundled binaries/images: Non-product generated or binary artifacts were excluded from the static implementation audit.
- Excluded examples/binarylight/binarylight_device.c, examples/clock/clock_device.c, test/TestDevice.c, std/av/src/cybergarage/upnp/std/av/renderer/cavtransport_service.c, std/av/src/cybergarage/upnp/std/av/renderer/cconnectionmgrr_service.c, std/av/src/cybergarage/upnp/std/av/renderer/cmediarenderer_device.c, std/av/src/cybergarage/upnp/std/av/renderer/crenderingcontrol_service.c, std/av/src/cybergarage/upnp/std/av/server/cconnectionmgr_service.c, std/av/src/cybergarage/upnp/std/av/server/ccontentdir_service.c, std/av/src/cybergarage/upnp/std/av/server/cmediaserver_device.c: Executable bodies were audited; large trusted fixed XML/HTML literal sections were not exhaustively reviewed.
- Excluded Historical Xcode/Visual Studio project configurations and ancillary platform UI headers: Build routes and relevant flags inspected selectively; not every historical configuration/declaration was fully reviewed.

### Scan Summary

| Field | Value |
| --- | --- |
| Scan outcome | completed |
| Reportable findings | 13 |
| Severity mix | high: 1, medium: 7, low: 5 |
| Confidence mix | high: 13 |
| Coverage | partial |
| Validation mode | static source trace |

Canonical artifacts: `scan-manifest.json`, `findings.json`, and `coverage.json`. This report is a deterministic projection of those files.

## Threat Model

mUPnP is an embeddable C library for UPnP devices and control points, with Objective-C wrappers and demonstration applications. Device startup creates HTTP and SSDP listeners and announces a device; control-point startup creates SSDP multicast/unicast and HTTP event listeners, discovers devices, fetches XML descriptions and SCPDs, and permits application-directed SOAP control. It runs with the embedding process's native memory, filesystem and network authority, rather than as an isolated service. Source: src/mupnp/device.c:1373-1419; src/mupnp/controlpoint.c:164-225; src/mupnp/controlpoint.c:585-624; src/mupnp/control/action_ctrl.c:85-110; wrapper/objc/mUPnP/CGUpnpControlPoint.m:28; wrapper/objc/mUPnP/CGUpnpControlPoint.m:51. Examples and tests are distinct from the library: CMakeLists.txt:144-154; examples/clock/clock_device.c:262-282.

### Assets

- Integrity and availability of the embedding native process and its application-owned device action callbacks. Network input is parsed inside this process and action listeners execute directly: src/mupnp/http/http_server.c:182-200; src/mupnp/control/action_ctrl.c:54-68; src/mupnp/xml/xml_parser_expat.c:178-195.
- Device and service XML descriptions, action arguments, discovered-device cache, state variables and subscription identifiers. Received notifications update state and call application listeners: src/mupnp/controlpoint_http_server.c:53-101.
- Host network authority and evented service values sent to subscriber-selected destinations. CALLBACK supplies the stored delivery URL; notification host, port and path come from that URL: src/mupnp/device_http_server.c:530-551; src/mupnp/event/subscriber.c:166-171; src/mupnp/event/notify_request.c:124-125; src/mupnp/event/notify_request.c:160-176.
- Optional local description-file reads and general file operations retain process filesystem authority; filenames are caller supplied: src/mupnp/device.c:286-307; src/mupnp/io/file.c:374-392; src/mupnp/io/file.c:439-455.
- Conditional CI credential and publication authority: CODECOV_TOKEN is passed to the Codecov action; GITHUB_TOKEN is passed to the GitHub Pages deployment action for ./doc/html. Source: .github/workflows/make.yml:34-40; .github/workflows/doxygen.yml:39-45.

### Trust Boundaries

- Network peers -\> device HTTP listener -\> parser and application actions. The listener binds configured host interfaces at port 38400 by default, incrementing on bind failure; MUPNP_NET_USE_ANYADDR changes the bind address to 0.0.0.0. GET/HEAD, POST, subscription and presentation requests have separate dispatch; custom HTTP/presentation/action listeners belong to the embedding application. Service URL and action routing are protocol controls, not actor authentication. Source: include/mupnp/device.h:56-59; src/mupnp/device.c:1388-1399; src/mupnp/http/http_server_list.c:71-84; src/mupnp/net/interface.c:68-81; src/mupnp/device_http_server.c:68-89; src/mupnp/device_http_server.c:318-356; src/mupnp/control/action_ctrl.c:54-68.
- SSDP peers -\> control-point discovery -\> host HTTP egress -\> XML model. SSDP LOCATION supplies the description URL; only HTTP URLs proceed through the description consumer, which uses the URL host/port/request and checks response success before parsing. SCPDURL is resolved as an absolute URL, against XML URLBase, or against the SSDP LOCATION directory. Source: src/mupnp/controlpoint.c:594-618; src/mupnp/device.c:248-273; src/mupnp/service.c:179-181; src/mupnp/service.c:1099-1178; src/mupnp/service.c:1196-1208; src/mupnp/service.c:254-282.
- Application-directed control -\> description-derived device endpoint. mupnp_action_post constructs and sends a SOAP request; the service control URL determines destination host/port and SOAP action arguments are serialized from application action objects. Device-description metadata remains peer supplied. Source: src/mupnp/control/action_ctrl.c:93-101; src/mupnp/control/action_request.c:229-248; src/mupnp/control/action_request.c:271-278; src/mupnp/service.c:168-170.
- Network subscriber -\> device subscription state -\> outbound notifications. The built-in handler checks event-subscription service URI, CALLBACK/SID/NT combinations and NT value, creates a SID, clamps timeout when a positive maximum is configured, and stores CALLBACK after angle-bracket trimming. Host/port/path consumers are independent of incoming peer address. Source: src/mupnp/device_http_server.c:465-511; src/mupnp/device_http_server.c:530-560; src/mupnp/event/subscriber.c:166-171; src/mupnp/event/notify_request.c:124-125.
- Network event sender -\> control-point HTTP callback -\> cached state and application observers. Callback is published using the selected interface, actual event port and /eventSub; defaults start at TCP 39500 and may increment. Built-in handling matches SID to a known service and checks sequence before applying properties and notifying listeners. Source: include/mupnp/controlpoint.h:49-52; src/mupnp/controlpoint.c:51-56; src/mupnp/controlpoint.c:185-197; src/mupnp/controlpoint_http_server.c:49-101.
- Caller-selected local file -\> optional XML parser. MUPNP_USE_CFILE gates description-file loading; no repository directory confinement is implied. The actual filename is passed through to fopen with process OS permissions. Source: src/mupnp/device.c:286-307; src/mupnp/io/file.c:374-392.
- Trusted source/build configuration -\> produced native library. Autoconf emits MUPNP_\* consumer macros. CMake advertises CG_\* options and emits CG_\* definitions instead, so those switches alone do not enable consumer paths guarded by MUPNP_\*. Source: CMakeLists.txt:9-24; configure.ac:128-149; configure.ac:279-319; src/mupnp/http/http_server_list.c:72-77; src/mupnp/xml/xml_parser_expat.c:22; src/mupnp/xml/xml_parser_libxml2.c:22; include/mupnp/net/socket.h:18.
- Repository source -\> CI runner and conditional external reporting/publication. Build CI triggers on master pushes and pull requests and executes repository bootstrap/configure/make; documentation publication triggers on master pushes and supplies GITHUB_TOKEN to the deployment action. Source: .github/workflows/make.yml:3-30; .github/workflows/doxygen.yml:7-9; .github/workflows/doxygen.yml:39-45.

### Attacker Capabilities

- A peer with network reachability can submit SSDP packets, device HTTP/SOAP/subscription requests and control-point notifications. Exact reachability depends on host interfaces, firewall and embedding deployment; LAN-only isolation is not enforced merely by the README's home-network description. Source: src/mupnp/ssdp/ssdp_server.c:65-78; src/mupnp/http/http_server_list.c:71-84; src/mupnp/device_http_server.c:76-89.
- An advertised device can control LOCATION, XML URLBase, service URLs and HTTP description/SCPD contents, which cross into control-point HTTP egress and native XML parsing. This actor is not assumed to control the embedding application, OS account or trusted build flags. Source: src/mupnp/controlpoint.c:594-618; src/mupnp/service.c:1196-1208.
- A network subscriber supplies CALLBACK and timeout to the built-in device eventing path. New authority from a boundary failure would need to exceed the intended ability to receive evented service data at a callback; deployment-specific internal destinations and sensitive event values remain prerequisites. Source: src/mupnp/device_http_server.c:530-560.
- A caller can supply parser buffers, action objects, listener callbacks and optional filenames through the public C API. Callers normally already possess the same native process authority; caller choices are not themselves privilege escalation. Source: src/mupnp/device.c:288-307; src/mupnp/control/action_ctrl.c:85-110.
- A pull-request author controls submitted source for CI execution but is not assumed to possess maintainer merge rights, repository secrets or master-only publication authority. Source: .github/workflows/make.yml:3-7; .github/workflows/doxygen.yml:7-9.

### Security Objectives

- Preserve embedding-process memory safety and availability while parsing network HTTP, SSDP and XML input; enforce bounds before native allocations and updates. Source consumers: src/mupnp/http/http_packet.c:450-473; src/mupnp/xml/xml_parser_expat.c:178-195.
- Preserve application-defined control policy when network requests route to presentation, action and query callbacks; callers must understand that these callbacks run with process authority. Source: src/mupnp/device_http_server.c:68-83; src/mupnp/control/action_ctrl.c:54-68.
- Keep outgoing description, control and notification destinations bound to the intended peer or application policy, including host, port and derived request path. Source: src/mupnp/device.c:251-261; src/mupnp/service.c:1099-1208; src/mupnp/event/subscriber.c:166-171; src/mupnp/event/notify_request.c:124-125.
- Apply notifications only to the intended known subscription and preserve coherent service state; existing component controls include SID matching and sequence checks. Source: src/mupnp/controlpoint_http_server.c:53-89.
- Ensure build configuration establishes the advertised network, file, TLS and XML backend behavior at actual consumers, and preserve CI credential and publication target separation. Source: CMakeLists.txt:9-24; configure.ac:279-319; .github/workflows/make.yml:34-40; .github/workflows/doxygen.yml:39-45.

### Assumptions

- User context: 'Check the project'; no supplied threat model, deployment knowledge base or root SECURITY.md. Root policy resolution returned no policy. This architecture pass is read-only source mapping and does not constitute security-audit coverage.
- Actual embedding applications, network exposure, sensitive device actions/event values, OS privileges, runtime library versions and effective compile flags are unspecified. No multi-tenant boundary, credential store, admin service or sandbox should be invented.
- CMake configuration discrepancy: CG_\* advertised options are emitted as CG_\* definitions, but inspected consumers use MUPNP_\* gates; no source alias was found. Therefore CMake option values do not demonstrate ANYADDR, TLS, CFILE or XML backend activation. Autoconf uses the expected MUPNP_\* macros. Source: CMakeLists.txt:9-24; configure.ac:128-149; configure.ac:279-319; src/mupnp/device.c:286; src/mupnp/http/http_server_list.c:72; include/mupnp/net/socket.h:18; src/mupnp/xml/xml_parser_libxml2.c:22.
- Autoconf --enable-anyaddr help claims default yes, but the definition is added only in the provided-option yes branch; omission supplies no default action. Without external flags, omitted option follows per-interface consumers. Source: configure.ac:284-294; src/mupnp/http/http_server_list.c:72-77.
- Control-point API documentation says required ports may already be in use, while startup increments HTTP event port without an explicit bound and retries SSDP response ports through configured start +80. Effective listener/callback ports must be read after startup. Source: include/mupnp/controlpoint.h:219-224; src/mupnp/controlpoint.c:185-216.
- Expat is selected by its source guard when HAVE_CONFIG_H and TARGET_OS_IPHONE are absent; libxml2 has a different platform guard. Runtime parser version and entity/network behavior require later source/dependency validation, not assumptions from package names. Source: src/mupnp/xml/xml_parser_expat.c:22; src/mupnp/xml/xml_parser_libxml2.c:22; CMakeLists.txt:190-207.
- Objective-C wrappers use the same C objects/start APIs, and Unix/Windows clock startup uses the same device API; no distinct isolation boundary is established by wrappers or sample platform projects. Source: wrapper/objc/mUPnP/CGUpnpControlPoint.m:28; wrapper/objc/mUPnP/CGUpnpControlPoint.m:51; examples/clock/unix/clock_main.c:52; examples/clock/win32/vs2005/clock_main_win32.c:37.

## Findings

| Finding | Severity | Confidence | Detailed write-up |
| --- | --- | --- | --- |
| [HTTP read errors become negative heap offsets](#finding-1) | high | high | inline below |
| [Truncated HTTP body produces an infinite CPU loop](#finding-2) | medium | high | inline below |
| [Objective-C device wrappers retain pointers freed by SSDP removal](#finding-3) | medium | high | inline below |
| [POSIX worker shutdown frees live thread and server state](#finding-4) | medium | high | inline below |
| [Libxml2 parsing uses a byte length larger than its input copy](#finding-5) | medium | high | inline below |
| [Native HTTPS accepts unverified peer certificates](#finding-6) | medium | high | inline below |
| [Action wrappers leave a dangling Objective-C callback target](#finding-7) | medium | high | inline below |
| [Deep XML input can exhaust the stack during tree cleanup](#finding-8) | medium | high | inline below |
| [Unbounded M-SEARCH MX blocks the SSDP worker](#finding-9) | low | high | inline below |
| [Test presentation handler sends uninitialized stack contents](#finding-10) | low | high | inline below |
| [Media directory dump follows container cycles without a bound](#finding-11) | low | high | inline below |
| [Malformed SSDP packets leak tokenizers without bound](#finding-12) | low | high | inline below |
| [A zero-length UDP packet permanently stops SSDP reception](#finding-13) | low | high | inline below |

### Confidence Scale

| Label | Meaning |
| --- | --- |
| high | Direct evidence supports the finding with no material unresolved blocker. |
| medium | Evidence supports a plausible issue, but material runtime or reachability proof remains. |
| low | Evidence is incomplete and the item is retained only for explicit follow-up. |

<a id="finding-1"></a>

### [1] HTTP read errors become negative heap offsets

| Field | Value |
| --- | --- |
| Severity | high |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | memory-safety |
| CWE | CWE-787 |
| Affected lines | src/mupnp/http/http_request.c:740-745, src/mupnp/http/http_packet.c:462-468, src/mupnp/http/http_packet.c:414-422, src/mupnp/net/socket.c:701-713 |

#### Summary

A TCP peer delays a declared body past receive timeout; recv returns -1 which decreases readLen and the next receive pointer precedes the allocation. Native request and response readers share this code; chunk reader has the same operation. Receive timeout and retry limit do not preserve a nonnegative offset.

#### Root Cause

A TCP peer delays a declared body past receive timeout; recv returns -1 which decreases readLen and the next receive pointer precedes the allocation. Native request and response readers share this code; chunk reader has the same operation. Receive timeout and retry limit do not preserve a nonnegative offset.

**http input** — `src/mupnp/http/http_request.c:740-745`

The network request parser invokes the shared body reader when framing headers are present.

```c
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
           in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));
```

**http read** — `src/mupnp/http/http_packet.c:462-468`

A signed receive result changes the accumulated byte offset before it is checked.

```c
    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }
```

**http chunk** — `src/mupnp/http/http_packet.c:414-422`

The chunk reader shares the signed-offset error and appends the accumulated result.

```c
  readLen = 0;
  /* Read content until conLen is reached, or tired of trying */
  while (readLen < conLen && tries < 20) {
    readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
    tries++;
  }

  /* Append content to packet */
  mupnp_http_packet_appendncontent(httpPkt, content, readLen);
```

**socket recv** — `src/mupnp/net/socket.c:701-713`

Native recv/SSL_read returns signed error or EOF values directly.

```c
    recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
  recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
  recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
  recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(MUPNP_USE_OPENSSL)
  }
  else {
    recvLen = SSL_read(sock->ssl, buffer, bufferLen);
```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. A TCP peer delays a declared body past receive timeout; recv returns -1 which decreases readLen and the next receive pointer precedes the allocation. Native request and response readers share this code; chunk reader has the same operation. Receive timeout and retry limit do not preserve a nonnegative offset.

Validation method: static source trace

**http input** — `src/mupnp/http/http_request.c:740-745`

The network request parser invokes the shared body reader when framing headers are present.

```c
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
           in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));
```

**http read** — `src/mupnp/http/http_packet.c:462-468`

A signed receive result changes the accumulated byte offset before it is checked.

```c
    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }
```

**http chunk** — `src/mupnp/http/http_packet.c:414-422`

The chunk reader shares the signed-offset error and appends the accumulated result.

```c
  readLen = 0;
  /* Read content until conLen is reached, or tired of trying */
  while (readLen < conLen && tries < 20) {
    readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
    tries++;
  }

  /* Append content to packet */
  mupnp_http_packet_appendncontent(httpPkt, content, readLen);
```

**socket recv** — `src/mupnp/net/socket.c:701-713`

Native recv/SSL_read returns signed error or EOF values directly.

```c
    recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
  recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
  recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
  recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(MUPNP_USE_OPENSSL)
  }
  else {
    recvLen = SSL_read(sock->ssl, buffer, bufferLen);
```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

http-input -\> http-read -\> http-chunk -\> socket-recv

**http input** — `src/mupnp/http/http_request.c:740-745`

The network request parser invokes the shared body reader when framing headers are present.

```c
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
           in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));
```

**http read** — `src/mupnp/http/http_packet.c:462-468`

A signed receive result changes the accumulated byte offset before it is checked.

```c
    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }
```

**http chunk** — `src/mupnp/http/http_packet.c:414-422`

The chunk reader shares the signed-offset error and appends the accumulated result.

```c
  readLen = 0;
  /* Read content until conLen is reached, or tired of trying */
  while (readLen < conLen && tries < 20) {
    readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
    tries++;
  }

  /* Append content to packet */
  mupnp_http_packet_appendncontent(httpPkt, content, readLen);
```

**socket recv** — `src/mupnp/net/socket.c:701-713`

Native recv/SSL_read returns signed error or EOF values directly.

```c
    recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
  recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
  recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
  recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(MUPNP_USE_OPENSSL)
  }
  else {
    recvLen = SSL_read(sock->ssl, buffer, bufferLen);
```

#### Reachability

Timing requires one receive error followed by more peer-controlled data; reachable on exposed native HTTP listeners.

#### Severity

**High** — Timing requires one receive error followed by more peer-controlled data; reachable on exposed native HTTP listeners.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** high
- **Why:** Timing requires one receive error followed by more peer-controlled data; reachable on exposed native HTTP listeners.

Likelihood assessment:
- **Level:** high
- **Why:** Timing requires one receive error followed by more peer-controlled data; reachable on exposed native HTTP listeners.

#### Remediation

Keep receive result separate; fail and free on nonpositive result; accumulate only positive bounded reads.

Tests:
- Keep receive result separate; fail and free on nonpositive result; accumulate only positive bounded reads. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

<a id="finding-2"></a>

### [2] Truncated HTTP body produces an infinite CPU loop

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | denial-of-service |
| CWE | CWE-835 |
| Affected lines | src/mupnp/http/http_request.c:740-745, src/mupnp/http/http_packet.c:462-468, src/mupnp/net/socket.c:701-713 |

#### Summary

After positive partial body data, EOF repeatedly returns zero. Accumulated readLen stays positive so tries does not increase. Timeouts do not bound immediate EOF and runnableFlag is never checked inside the loop. Repeated connections consume CPU and workers.

#### Root Cause

After positive partial body data, EOF repeatedly returns zero. Accumulated readLen stays positive so tries does not increase. Timeouts do not bound immediate EOF and runnableFlag is never checked inside the loop. Repeated connections consume CPU and workers.

**http input** — `src/mupnp/http/http_request.c:740-745`

The network request parser invokes the shared body reader when framing headers are present.

```c
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
           in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));
```

**http read** — `src/mupnp/http/http_packet.c:462-468`

A signed receive result changes the accumulated byte offset before it is checked.

```c
    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }
```

**socket recv** — `src/mupnp/net/socket.c:701-713`

Native recv/SSL_read returns signed error or EOF values directly.

```c
    recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
  recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
  recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
  recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(MUPNP_USE_OPENSSL)
  }
  else {
    recvLen = SSL_read(sock->ssl, buffer, bufferLen);
```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. After positive partial body data, EOF repeatedly returns zero. Accumulated readLen stays positive so tries does not increase. Timeouts do not bound immediate EOF and runnableFlag is never checked inside the loop. Repeated connections consume CPU and workers.

Validation method: static source trace

**http input** — `src/mupnp/http/http_request.c:740-745`

The network request parser invokes the shared body reader when framing headers are present.

```c
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
           in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));
```

**http read** — `src/mupnp/http/http_packet.c:462-468`

A signed receive result changes the accumulated byte offset before it is checked.

```c
    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }
```

**socket recv** — `src/mupnp/net/socket.c:701-713`

Native recv/SSL_read returns signed error or EOF values directly.

```c
    recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
  recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
  recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
  recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(MUPNP_USE_OPENSSL)
  }
  else {
    recvLen = SSL_read(sock->ssl, buffer, bufferLen);
```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

http-input -\> http-read -\> socket-recv

**http input** — `src/mupnp/http/http_request.c:740-745`

The network request parser invokes the shared body reader when framing headers are present.

```c
  mupnp_http_packet_read_headers((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));

  /* HTTP-request must have Content-Length or Transfer-Encoding header
           in order to have body */
  if (mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_CONTENT_LENGTH) || mupnp_http_packet_hasheader((mUpnpHttpPacket*)httpReq, MUPNP_HTTP_TRANSFER_ENCODING))
    mupnp_http_packet_read_body((mUpnpHttpPacket*)httpReq, sock, lineBuf, sizeof(lineBuf));
```

**http read** — `src/mupnp/http/http_packet.c:462-468`

A signed receive result changes the accumulated byte offset before it is checked.

```c
    /* Read content until conLen is reached, or tired of trying */
    while (readLen < conLen && tries < 20) {
      readLen += mupnp_socket_read(sock, (content + readLen), (conLen - readLen));
      /* Fixed to increment the counter only when mupnp_socket_read() doesn't read data */
      if (readLen <= 0)
        tries++;
    }
```

**socket recv** — `src/mupnp/net/socket.c:701-713`

Native recv/SSL_read returns signed error or EOF values directly.

```c
    recvLen = so_recv(sock->id, buffer, bufferLen, 0);
#elif defined(TENGINE) && defined(MUPNP_TENGINE_NET_KASAGO)
  recvLen = ka_recv(sock->id, buffer, bufferLen, 0);
#elif defined(ITRON)
  recvLen = tcp_rcv_dat(sock->id, buffer, bufferLen, TMO_FEVR);
#else
  recvLen = recv(sock->id, buffer, bufferLen, 0);
#endif

#if defined(MUPNP_USE_OPENSSL)
  }
  else {
    recvLen = SSL_read(sock->ssl, buffer, bufferLen);
```

#### Reachability

A network peer can retain a worker in a busy loop; repeated connections affect service availability.

#### Severity

**Medium** — A network peer can retain a worker in a busy loop; repeated connections affect service availability.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** medium
- **Why:** A network peer can retain a worker in a busy loop; repeated connections affect service availability.

Likelihood assessment:
- **Level:** high
- **Why:** A network peer can retain a worker in a busy loop; repeated connections affect service availability.

#### Remediation

Stop immediately on EOF before completing the declared body.

Tests:
- Stop immediately on EOF before completing the declared body. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

<a id="finding-3"></a>

### [3] Objective-C device wrappers retain pointers freed by SSDP removal

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified actual source flow and counterevidence; dynamic exploitation was not tested. |
| Category | use-after-free |
| CWE | CWE-416 |
| Affected lines | src/mupnp/controlpoint.c:863-867, wrapper/objc/mUPnP/CGUpnpControlPoint.m:103-113, wrapper/objc/mUPnP/CGUpnpDevice.m:41-48, src/mupnp/controlpoint.c:717-725, wrapper/objc/mUPnP/CGUpnpDevice.m:78-86 |

#### Summary

Control-point enumeration returns retainable Objective-C wrappers around borrowed cache-device pointers. SSDP removal deletes the C device without invalidating those wrappers. Later property and subordinate-object access dereferences freed memory; the iOS browser retains and later reads such wrappers.

#### Root Cause

Control-point enumeration returns retainable Objective-C wrappers around borrowed cache-device pointers. SSDP removal deletes the C device without invalidating those wrappers. Later property and subordinate-object access dereferences freed memory; the iOS browser retains and later reads such wrappers.

**device byebye** — `src/mupnp/controlpoint.c:863-867`

A peer departure notification enters cache removal.

```c
  if (mupnp_ssdp_packet_isrootdevice(ssdpPkt) == true) {
    if (mupnp_ssdp_packet_isalive(ssdpPkt) == true)
      mupnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
    else if (mupnp_ssdp_packet_isbyebye(ssdpPkt) == true)
      mupnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
```

**objc list** — `wrapper/objc/mUPnP/CGUpnpControlPoint.m:103-113`

Enumeration creates externally retainable wrappers of live cache pointers.

```objective-c
    return [NSArray array];
  NSMutableArray* devArray = [NSMutableArray array];
  mUpnpDevice* cDevice;
  for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
    CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
    [devArray addObject:device];
  }
  return devArray;
}

- (CGUpnpDevice*)deviceForUDN:(NSString*)udn
```

**objc borrow** — `wrapper/objc/mUPnP/CGUpnpDevice.m:41-48`

The wrapper stores a borrowed pointer without extending or tracking its lifetime.

```objective-c
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  isCObjectCreated = NO;
  return self;
}

```

**device remove** — `src/mupnp/controlpoint.c:717-725`

The underlying C device is deleted after the notification; wrappers are not invalidated.

```c
  if (listener != NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    listener(ctrlPoint, udn, mUpnpDeviceStatusRemoved);
    mupnp_controlpoint_lock(ctrlPoint);
  }

  mupnp_device_delete(dev);

  mupnp_controlpoint_unlock(ctrlPoint);
```

**objc read** — `wrapper/objc/mUPnP/CGUpnpDevice.m:78-86`

Non-NULL dangling cObject is dereferenced by ordinary property access.

```objective-c
- (NSString*)friendlyName
{
  if (!cObject)
    return nil;
  const char* name = mupnp_device_getfriendlyname(cObject);
  if (name) {
    return [NSString stringWithUTF8String:name];
  }
  return nil;
```

#### Validation

Control-point enumeration returns retainable Objective-C wrappers around borrowed cache-device pointers. SSDP removal deletes the C device without invalidating those wrappers. Later property and subordinate-object access dereferences freed memory; the iOS browser retains and later reads such wrappers.

Validation method: static source trace

**device byebye** — `src/mupnp/controlpoint.c:863-867`

A peer departure notification enters cache removal.

```c
  if (mupnp_ssdp_packet_isrootdevice(ssdpPkt) == true) {
    if (mupnp_ssdp_packet_isalive(ssdpPkt) == true)
      mupnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
    else if (mupnp_ssdp_packet_isbyebye(ssdpPkt) == true)
      mupnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
```

**objc list** — `wrapper/objc/mUPnP/CGUpnpControlPoint.m:103-113`

Enumeration creates externally retainable wrappers of live cache pointers.

```objective-c
    return [NSArray array];
  NSMutableArray* devArray = [NSMutableArray array];
  mUpnpDevice* cDevice;
  for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
    CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
    [devArray addObject:device];
  }
  return devArray;
}

- (CGUpnpDevice*)deviceForUDN:(NSString*)udn
```

**objc borrow** — `wrapper/objc/mUPnP/CGUpnpDevice.m:41-48`

The wrapper stores a borrowed pointer without extending or tracking its lifetime.

```objective-c
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  isCObjectCreated = NO;
  return self;
}

```

**device remove** — `src/mupnp/controlpoint.c:717-725`

The underlying C device is deleted after the notification; wrappers are not invalidated.

```c
  if (listener != NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    listener(ctrlPoint, udn, mUpnpDeviceStatusRemoved);
    mupnp_controlpoint_lock(ctrlPoint);
  }

  mupnp_device_delete(dev);

  mupnp_controlpoint_unlock(ctrlPoint);
```

**objc read** — `wrapper/objc/mUPnP/CGUpnpDevice.m:78-86`

Non-NULL dangling cObject is dereferenced by ordinary property access.

```objective-c
- (NSString*)friendlyName
{
  if (!cObject)
    return nil;
  const char* name = mupnp_device_getfriendlyname(cObject);
  if (name) {
    return [NSString stringWithUTF8String:name];
  }
  return nil;
```

Limitations:
- No application code or vulnerability-triggering input was executed.

#### Dataflow

device-byebye -\> objc-list -\> objc-borrow -\> device-remove -\> objc-read

**device byebye** — `src/mupnp/controlpoint.c:863-867`

A peer departure notification enters cache removal.

```c
  if (mupnp_ssdp_packet_isrootdevice(ssdpPkt) == true) {
    if (mupnp_ssdp_packet_isalive(ssdpPkt) == true)
      mupnp_controlpoint_adddevicebyssdppacket(ctrlPoint, ssdpPkt);
    else if (mupnp_ssdp_packet_isbyebye(ssdpPkt) == true)
      mupnp_controlpoint_removedevicebyssdppacket(ctrlPoint, ssdpPkt);
```

**objc list** — `wrapper/objc/mUPnP/CGUpnpControlPoint.m:103-113`

Enumeration creates externally retainable wrappers of live cache pointers.

```objective-c
    return [NSArray array];
  NSMutableArray* devArray = [NSMutableArray array];
  mUpnpDevice* cDevice;
  for (cDevice = mupnp_controlpoint_getdevices(cObject); cDevice; cDevice = mupnp_device_next(cDevice)) {
    CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
    [devArray addObject:device];
  }
  return devArray;
}

- (CGUpnpDevice*)deviceForUDN:(NSString*)udn
```

**objc borrow** — `wrapper/objc/mUPnP/CGUpnpDevice.m:41-48`

The wrapper stores a borrowed pointer without extending or tracking its lifetime.

```objective-c
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  isCObjectCreated = NO;
  return self;
}

```

**device remove** — `src/mupnp/controlpoint.c:717-725`

The underlying C device is deleted after the notification; wrappers are not invalidated.

```c
  if (listener != NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    listener(ctrlPoint, udn, mUpnpDeviceStatusRemoved);
    mupnp_controlpoint_lock(ctrlPoint);
  }

  mupnp_device_delete(dev);

  mupnp_controlpoint_unlock(ctrlPoint);
```

**objc read** — `wrapper/objc/mUPnP/CGUpnpDevice.m:78-86`

Non-NULL dangling cObject is dereferenced by ordinary property access.

```objective-c
- (NSString*)friendlyName
{
  if (!cObject)
    return nil;
  const char* name = mupnp_device_getfriendlyname(cObject);
  if (name) {
    return [NSString stringWithUTF8String:name];
  }
  return nil;
```

#### Reachability

Requires Objective-C wrapper use across asynchronous removal. Core cache locking does not protect external wrapper getters; an application that never retains wrappers across removal avoids the specific trigger. Arbitrary code execution was not established.

#### Severity

**Medium** — Requires Objective-C wrapper use across asynchronous removal. Core cache locking does not protect external wrapper getters; an application that never retains wrappers across removal avoids the specific trigger. Arbitrary code execution was not established.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** high
- **Why:** Control-point enumeration returns retainable Objective-C wrappers around borrowed cache-device pointers. SSDP removal deletes the C device without invalidating those wrappers. Later property and subordinate-object access dereferences freed memory; the iOS browser retains and later reads such wrappers.

Likelihood assessment:
- **Level:** medium
- **Why:** Requires Objective-C wrapper use across asynchronous removal. Core cache locking does not protect external wrapper getters; an application that never retains wrappers across removal avoids the specific trigger. Arbitrary code execution was not established.

#### Remediation

Give wrappers refcounted native ownership or a shared invalidatable handle checked under the control-point lock; share that lifetime control with service, action, state-variable and icon wrappers.

Tests:
- Retain a device and subordinate wrapper across simulated cache removal and confirm every later access returns safely.

Preventive controls:
- Centralize and enforce the repaired lifetime or input-limit invariant.

<a id="finding-4"></a>

### [4] POSIX worker shutdown frees live thread and server state

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | use-after-free |
| CWE | CWE-416 |
| Affected lines | src/mupnp/http/http_server.c:320-330, src/mupnp/util/thread.c:451-463, src/mupnp/util/thread.c:262-268, src/mupnp/http/http_server.c:218-230 |

#### Summary

Detached worker stop sets runnableFlag, probes liveness with signal 0 and sleeps instead of waiting for completion. thread_delete frees state. Delayed HTTP worker resumes during legitimate shutdown and reads freed thread/server state; requires lifecycle transition. Sleeping is not lifetime synchronization.

#### Root Cause

Detached worker stop sets runnableFlag, probes liveness with signal 0 and sleeps instead of waiting for completion. thread_delete frees state. Delayed HTTP worker resumes during legitimate shutdown and reads freed thread/server state; requires lifecycle transition. Sleeping is not lifetime synchronization.

**http stop** — `src/mupnp/http/http_server.c:320-330`

Server stop destroys the worker list following asynchronous stop.

```c
  if (httpServer->acceptThread != NULL) {
    mupnp_thread_stop(httpServer->acceptThread);
    mupnp_thread_delete(httpServer->acceptThread);
    httpServer->acceptThread = NULL;
  }
  /**** Thanks for Makela Aapo (10/31/05) ****/
  if (httpServer->clientThreads != NULL) {
    mupnp_threadlist_stop(httpServer->clientThreads);
    mupnp_threadlist_delete(httpServer->clientThreads);
    httpServer->clientThreads = NULL;
  }
```

**thread stop** — `src/mupnp/util/thread.c:451-463`

Signal zero and a sleep do not establish worker exit.

```c
#elif defined(TENGINE) && defined(PROCESS_BASE)
      b_ter_tsk(thread->taskID);
#else
      mupnp_log_debug_s("Killing thread %p\n", thread);
      pthread_kill(thread->pThread, 0);
      /* MODIFICATION Fabrice Fontaine Orange 24/04/2007
                mupnp_log_debug_s("Thread %p signalled, joining.\n", thread);
                pthread_join(thread->pThread, NULL);
                mupnp_log_debug_s("Thread %p joined.\n", thread); */
      /* Now we wait one second for thread termination instead of using pthread_join */
      mupnp_sleep(MUPNP_THREAD_MIN_SLEEP);
/* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
#endif
```

**thread free** — `src/mupnp/util/thread.c:262-268`

Thread storage is freed without waiting for completion.

```c
  if (thread->runnableFlag == true)
    mupnp_thread_stop(thread);

  mupnp_thread_remove(thread);

  free(thread);

```

**http cleanup** — `src/mupnp/http/http_server.c:218-230`

A returning worker removes/frees its thread and accesses the owner mutex.

```c
  mupnp_socket_close(clientSock);
  mupnp_socket_delete(clientSock);

  mupnp_http_server_clientdata_delete(clientData);
  mupnp_thread_setuserdata(thread, NULL);

  // This code frequently crashes. mutex lock referencing free'd memory.
  mupnp_http_server_lock(httpServer);
  mupnp_thread_remove(thread);
  mupnp_http_server_unlock(httpServer);

  mupnp_log_debug_l4("Leaving...\n");

```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. Detached worker stop sets runnableFlag, probes liveness with signal 0 and sleeps instead of waiting for completion. thread_delete frees state. Delayed HTTP worker resumes during legitimate shutdown and reads freed thread/server state; requires lifecycle transition. Sleeping is not lifetime synchronization.

Validation method: static source trace

**http stop** — `src/mupnp/http/http_server.c:320-330`

Server stop destroys the worker list following asynchronous stop.

```c
  if (httpServer->acceptThread != NULL) {
    mupnp_thread_stop(httpServer->acceptThread);
    mupnp_thread_delete(httpServer->acceptThread);
    httpServer->acceptThread = NULL;
  }
  /**** Thanks for Makela Aapo (10/31/05) ****/
  if (httpServer->clientThreads != NULL) {
    mupnp_threadlist_stop(httpServer->clientThreads);
    mupnp_threadlist_delete(httpServer->clientThreads);
    httpServer->clientThreads = NULL;
  }
```

**thread stop** — `src/mupnp/util/thread.c:451-463`

Signal zero and a sleep do not establish worker exit.

```c
#elif defined(TENGINE) && defined(PROCESS_BASE)
      b_ter_tsk(thread->taskID);
#else
      mupnp_log_debug_s("Killing thread %p\n", thread);
      pthread_kill(thread->pThread, 0);
      /* MODIFICATION Fabrice Fontaine Orange 24/04/2007
                mupnp_log_debug_s("Thread %p signalled, joining.\n", thread);
                pthread_join(thread->pThread, NULL);
                mupnp_log_debug_s("Thread %p joined.\n", thread); */
      /* Now we wait one second for thread termination instead of using pthread_join */
      mupnp_sleep(MUPNP_THREAD_MIN_SLEEP);
/* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
#endif
```

**thread free** — `src/mupnp/util/thread.c:262-268`

Thread storage is freed without waiting for completion.

```c
  if (thread->runnableFlag == true)
    mupnp_thread_stop(thread);

  mupnp_thread_remove(thread);

  free(thread);

```

**http cleanup** — `src/mupnp/http/http_server.c:218-230`

A returning worker removes/frees its thread and accesses the owner mutex.

```c
  mupnp_socket_close(clientSock);
  mupnp_socket_delete(clientSock);

  mupnp_http_server_clientdata_delete(clientData);
  mupnp_thread_setuserdata(thread, NULL);

  // This code frequently crashes. mutex lock referencing free'd memory.
  mupnp_http_server_lock(httpServer);
  mupnp_thread_remove(thread);
  mupnp_http_server_unlock(httpServer);

  mupnp_log_debug_l4("Leaving...\n");

```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

http-stop -\> thread-stop -\> thread-free -\> http-cleanup

**http stop** — `src/mupnp/http/http_server.c:320-330`

Server stop destroys the worker list following asynchronous stop.

```c
  if (httpServer->acceptThread != NULL) {
    mupnp_thread_stop(httpServer->acceptThread);
    mupnp_thread_delete(httpServer->acceptThread);
    httpServer->acceptThread = NULL;
  }
  /**** Thanks for Makela Aapo (10/31/05) ****/
  if (httpServer->clientThreads != NULL) {
    mupnp_threadlist_stop(httpServer->clientThreads);
    mupnp_threadlist_delete(httpServer->clientThreads);
    httpServer->clientThreads = NULL;
  }
```

**thread stop** — `src/mupnp/util/thread.c:451-463`

Signal zero and a sleep do not establish worker exit.

```c
#elif defined(TENGINE) && defined(PROCESS_BASE)
      b_ter_tsk(thread->taskID);
#else
      mupnp_log_debug_s("Killing thread %p\n", thread);
      pthread_kill(thread->pThread, 0);
      /* MODIFICATION Fabrice Fontaine Orange 24/04/2007
                mupnp_log_debug_s("Thread %p signalled, joining.\n", thread);
                pthread_join(thread->pThread, NULL);
                mupnp_log_debug_s("Thread %p joined.\n", thread); */
      /* Now we wait one second for thread termination instead of using pthread_join */
      mupnp_sleep(MUPNP_THREAD_MIN_SLEEP);
/* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
#endif
```

**thread free** — `src/mupnp/util/thread.c:262-268`

Thread storage is freed without waiting for completion.

```c
  if (thread->runnableFlag == true)
    mupnp_thread_stop(thread);

  mupnp_thread_remove(thread);

  free(thread);

```

**http cleanup** — `src/mupnp/http/http_server.c:218-230`

A returning worker removes/frees its thread and accesses the owner mutex.

```c
  mupnp_socket_close(clientSock);
  mupnp_socket_delete(clientSock);

  mupnp_http_server_clientdata_delete(clientData);
  mupnp_thread_setuserdata(thread, NULL);

  // This code frequently crashes. mutex lock referencing free'd memory.
  mupnp_http_server_lock(httpServer);
  mupnp_thread_remove(thread);
  mupnp_http_server_unlock(httpServer);

  mupnp_log_debug_l4("Leaving...\n");

```

#### Reachability

Memory corruption requires a legitimate shutdown/restart while a peer keeps a worker active.

#### Severity

**Medium** — Memory corruption requires a legitimate shutdown/restart while a peer keeps a worker active.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** high
- **Why:** Memory corruption requires a legitimate shutdown/restart while a peer keeps a worker active.

Likelihood assessment:
- **Level:** medium
- **Why:** Memory corruption requires a legitimate shutdown/restart while a peer keeps a worker active.

#### Remediation

Unblock sockets and synchronize worker completion before releasing worker or owner state.

Tests:
- Unblock sockets and synchronize worker completion before releasing worker or owner state. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

<a id="finding-5"></a>

### [5] Libxml2 parsing uses a byte length larger than its input copy

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified actual source flow and counterevidence; dynamic exploitation was not tested. |
| Category | memory-safety |
| CWE | CWE-125, CWE-787 |
| Affected lines | src/mupnp/soap/soap_request.c:180-188, src/mupnp/soap/soap_request.c:202-207, src/mupnp/xml/xml_parser_libxml2.c:288-295, src/mupnp/xml/xml_parser_libxml2.c:355-358, src/mupnp/xml/xml_parser_libxml2.c:298-308, src/mupnp/xml/xml_parser_libxml2.c:182-191 |

#### Summary

The optional libxml2 backend copies HTTP XML with `mupnp_strdup`, which stops at NUL, then passes the original explicit body length to the parser and recovery loop. Embedded NUL bytes make the copied allocation shorter than the consumed range, permitting out-of-bounds reads and, on invalid-character recovery, writes.

#### Root Cause

The optional libxml2 backend copies HTTP XML with `mupnp_strdup`, which stops at NUL, then passes the original explicit body length to the parser and recovery loop. Embedded NUL bytes make the copied allocation shorter than the consumed range, permitting out-of-bounds reads and, on invalid-character recovery, writes.

**soap input** — `src/mupnp/soap/soap_request.c:180-188`

HTTP content and explicit framing length pass together into SOAP parsing.

```c
  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
```

**soap parse** — `src/mupnp/soap/soap_request.c:202-207`

SOAP forwards bytes and length to the active XML backend.

```c
  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);
```

**xml copy** — `src/mupnp/xml/xml_parser_libxml2.c:288-295`

The NUL-terminated duplicate is passed with the original explicit byte length.

```c
  char* data = mupnp_strdup(parseData);
  if (!data)
    return false;

  libxml2Data.rootNode = NULL;
  libxml2Data.currNode = NULL;

  retval = mupnp_libxml2_parsewrapper(&mupnpLibxml2Handler, &libxml2Data, data, len, LIBXML2_NOFLAGS);
```

**xml memory** — `src/mupnp/xml/xml_parser_libxml2.c:355-358`

The libxml2 context consumes that length from the shorter allocation.

```c
  if (sax == NULL)
    return -1;

  ctxt = xmlCreateMemoryParserCtxt(buffer, (int)size);
```

**xml recover** — `src/mupnp/xml/xml_parser_libxml2.c:298-308`

Invalid-character recovery traverses the duplicate using the same length.

```c
  case XML_ERR_INVALID_CHAR:
    mupnp_log_debug_s("Trying to recover from error %d.\n", retval);

    if (libxml2Data.rootNode != NULL)
      mupnp_xml_node_delete(libxml2Data.rootNode);

    libxml2Data.rootNode = NULL;
    libxml2Data.currNode = NULL;

    /* Replace non utf8 characters with '?' */
    mupnp_xml_force_utf8(data, len);
```

**xml recover loop** — `src/mupnp/xml/xml_parser_libxml2.c:182-191`

The recovery loop indexes through the supplied length, beyond a NUL-shortened allocation.

```c
  while (read <= len) {
    /* First we check if byte is one byte UTF8 character */
    if (UTF_RANGE1_1_R == (*(data + read) & UTF_RANGE1_1)) {
      read++;
      continue;
    }

    /* If not then we check if byte starts two byte sequence. */
    else if (UTF_RANGE2_1_R == (*(data + read) & UTF_RANGE2_1)) {
      /* We know that if this is correct two byte UTF8 char
```

#### Validation

The optional libxml2 backend copies HTTP XML with `mupnp_strdup`, which stops at NUL, then passes the original explicit body length to the parser and recovery loop. Embedded NUL bytes make the copied allocation shorter than the consumed range, permitting out-of-bounds reads and, on invalid-character recovery, writes.

Validation method: static source trace

**soap input** — `src/mupnp/soap/soap_request.c:180-188`

HTTP content and explicit framing length pass together into SOAP parsing.

```c
  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
```

**soap parse** — `src/mupnp/soap/soap_request.c:202-207`

SOAP forwards bytes and length to the active XML backend.

```c
  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);
```

**xml copy** — `src/mupnp/xml/xml_parser_libxml2.c:288-295`

The NUL-terminated duplicate is passed with the original explicit byte length.

```c
  char* data = mupnp_strdup(parseData);
  if (!data)
    return false;

  libxml2Data.rootNode = NULL;
  libxml2Data.currNode = NULL;

  retval = mupnp_libxml2_parsewrapper(&mupnpLibxml2Handler, &libxml2Data, data, len, LIBXML2_NOFLAGS);
```

**xml memory** — `src/mupnp/xml/xml_parser_libxml2.c:355-358`

The libxml2 context consumes that length from the shorter allocation.

```c
  if (sax == NULL)
    return -1;

  ctxt = xmlCreateMemoryParserCtxt(buffer, (int)size);
```

**xml recover** — `src/mupnp/xml/xml_parser_libxml2.c:298-308`

Invalid-character recovery traverses the duplicate using the same length.

```c
  case XML_ERR_INVALID_CHAR:
    mupnp_log_debug_s("Trying to recover from error %d.\n", retval);

    if (libxml2Data.rootNode != NULL)
      mupnp_xml_node_delete(libxml2Data.rootNode);

    libxml2Data.rootNode = NULL;
    libxml2Data.currNode = NULL;

    /* Replace non utf8 characters with '?' */
    mupnp_xml_force_utf8(data, len);
```

**xml recover loop** — `src/mupnp/xml/xml_parser_libxml2.c:182-191`

The recovery loop indexes through the supplied length, beyond a NUL-shortened allocation.

```c
  while (read <= len) {
    /* First we check if byte is one byte UTF8 character */
    if (UTF_RANGE1_1_R == (*(data + read) & UTF_RANGE1_1)) {
      read++;
      continue;
    }

    /* If not then we check if byte starts two byte sequence. */
    else if (UTF_RANGE2_1_R == (*(data + read) & UTF_RANGE2_1)) {
      /* We know that if this is correct two byte UTF8 char
```

Limitations:
- No application code or vulnerability-triggering input was executed.

#### Dataflow

soap-input -\> soap-parse -\> xml-copy -\> xml-memory -\> xml-recover -\> xml-recover-loop

**soap input** — `src/mupnp/soap/soap_request.c:180-188`

HTTP content and explicit framing length pass together into SOAP parsing.

```c
  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
```

**soap parse** — `src/mupnp/soap/soap_request.c:202-207`

SOAP forwards bytes and length to the active XML backend.

```c
  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);
```

**xml copy** — `src/mupnp/xml/xml_parser_libxml2.c:288-295`

The NUL-terminated duplicate is passed with the original explicit byte length.

```c
  char* data = mupnp_strdup(parseData);
  if (!data)
    return false;

  libxml2Data.rootNode = NULL;
  libxml2Data.currNode = NULL;

  retval = mupnp_libxml2_parsewrapper(&mupnpLibxml2Handler, &libxml2Data, data, len, LIBXML2_NOFLAGS);
```

**xml memory** — `src/mupnp/xml/xml_parser_libxml2.c:355-358`

The libxml2 context consumes that length from the shorter allocation.

```c
  if (sax == NULL)
    return -1;

  ctxt = xmlCreateMemoryParserCtxt(buffer, (int)size);
```

**xml recover** — `src/mupnp/xml/xml_parser_libxml2.c:298-308`

Invalid-character recovery traverses the duplicate using the same length.

```c
  case XML_ERR_INVALID_CHAR:
    mupnp_log_debug_s("Trying to recover from error %d.\n", retval);

    if (libxml2Data.rootNode != NULL)
      mupnp_xml_node_delete(libxml2Data.rootNode);

    libxml2Data.rootNode = NULL;
    libxml2Data.currNode = NULL;

    /* Replace non utf8 characters with '?' */
    mupnp_xml_force_utf8(data, len);
```

**xml recover loop** — `src/mupnp/xml/xml_parser_libxml2.c:182-191`

The recovery loop indexes through the supplied length, beyond a NUL-shortened allocation.

```c
  while (read <= len) {
    /* First we check if byte is one byte UTF8 character */
    if (UTF_RANGE1_1_R == (*(data + read) & UTF_RANGE1_1)) {
      read++;
      continue;
    }

    /* If not then we check if byte starts two byte sequence. */
    else if (UTF_RANGE2_1_R == (*(data + read) & UTF_RANGE2_1)) {
      /* We know that if this is correct two byte UTF8 char
```

#### Reachability

Requires the libxml2 backend (explicit MUPNP_XMLPARSER_LIBXML2 or iPhone guard); Expat does not use this copy. Normal NUL-free HTTP XML has consistent length. No memory disclosure channel or code execution was established.

#### Severity

**Medium** — Requires the libxml2 backend (explicit MUPNP_XMLPARSER_LIBXML2 or iPhone guard); Expat does not use this copy. Normal NUL-free HTTP XML has consistent length. No memory disclosure channel or code execution was established.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** high
- **Why:** The optional libxml2 backend copies HTTP XML with `mupnp_strdup`, which stops at NUL, then passes the original explicit body length to the parser and recovery loop. Embedded NUL bytes make the copied allocation shorter than the consumed range, permitting out-of-bounds reads and, on invalid-character recovery, writes.

Likelihood assessment:
- **Level:** medium
- **Why:** Requires the libxml2 backend (explicit MUPNP_XMLPARSER_LIBXML2 or iPhone guard); Expat does not use this copy. Normal NUL-free HTTP XML has consistent length. No memory disclosure channel or code execution was established.

#### Remediation

Copy exactly the explicit input length into a checked allocation, or reject embedded NUL before duplication; keep parser and UTF-8 recovery strictly within the owned buffer.

Tests:
- Verify embedded NUL and invalid-character inputs are rejected without reading or writing outside the allocation, using memory instrumentation.

Preventive controls:
- Centralize and enforce the repaired lifetime or input-limit invariant.

<a id="finding-6"></a>

### [6] Native HTTPS accepts unverified peer certificates

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | tls |
| CWE | CWE-295 |
| Affected lines | src/mupnp/http/http_request.c:352-356, src/mupnp/net/socket.c:668-680 |

#### Summary

Native HTTPS creates fresh SSL_CTX and handshakes without peer verification, trust roots or hostname validation. On-path attacker can modify HTTPS content. Requires MUPNP_USE_OPENSSL and public native HTTPS API; Curl is separate and defaults do not enable this backend.

#### Root Cause

Native HTTPS creates fresh SSL_CTX and handshakes without peer verification, trust roots or hostname validation. On-path attacker can modify HTTPS content. Requires MUPNP_USE_OPENSSL and public native HTTPS API; Curl is separate and defaults do not enable this backend.

**https entry** — `src/mupnp/http/http_request.c:352-356`

The public native HTTPS call selects secure socket transport.

```c
#if defined(MUPNP_USE_OPENSSL)
mUpnpHttpResponse* mupnp_https_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port)
{
  return mupnp_http_request_post_main(httpReq, ipaddr, port, true);
}
```

**tls context** — `src/mupnp/net/socket.c:668-680`

A fresh TLS context handshakes without configured peer or host verification.

```c
#if defined(MUPNP_USE_OPENSSL)
  if (mupnp_socket_isssl(sock) == true) {
    sock->ctx = SSL_CTX_new(SSLv23_client_method());
    sock->ssl = SSL_new(sock->ctx);
    if (SSL_set_fd(sock->ssl, mupnp_socket_getid(sock)) == 0) {
      mupnp_socket_close(sock);
      return false;
    }
    if (SSL_connect(sock->ssl) < 1) {
      mupnp_socket_close(sock);
      return false;
    }
  }
```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. Native HTTPS creates fresh SSL_CTX and handshakes without peer verification, trust roots or hostname validation. On-path attacker can modify HTTPS content. Requires MUPNP_USE_OPENSSL and public native HTTPS API; Curl is separate and defaults do not enable this backend.

Validation method: static source trace

**https entry** — `src/mupnp/http/http_request.c:352-356`

The public native HTTPS call selects secure socket transport.

```c
#if defined(MUPNP_USE_OPENSSL)
mUpnpHttpResponse* mupnp_https_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port)
{
  return mupnp_http_request_post_main(httpReq, ipaddr, port, true);
}
```

**tls context** — `src/mupnp/net/socket.c:668-680`

A fresh TLS context handshakes without configured peer or host verification.

```c
#if defined(MUPNP_USE_OPENSSL)
  if (mupnp_socket_isssl(sock) == true) {
    sock->ctx = SSL_CTX_new(SSLv23_client_method());
    sock->ssl = SSL_new(sock->ctx);
    if (SSL_set_fd(sock->ssl, mupnp_socket_getid(sock)) == 0) {
      mupnp_socket_close(sock);
      return false;
    }
    if (SSL_connect(sock->ssl) < 1) {
      mupnp_socket_close(sock);
      return false;
    }
  }
```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

https-entry -\> tls-context

**https entry** — `src/mupnp/http/http_request.c:352-356`

The public native HTTPS call selects secure socket transport.

```c
#if defined(MUPNP_USE_OPENSSL)
mUpnpHttpResponse* mupnp_https_request_post(mUpnpHttpRequest* httpReq, const char* ipaddr, int port)
{
  return mupnp_http_request_post_main(httpReq, ipaddr, port, true);
}
```

**tls context** — `src/mupnp/net/socket.c:668-680`

A fresh TLS context handshakes without configured peer or host verification.

```c
#if defined(MUPNP_USE_OPENSSL)
  if (mupnp_socket_isssl(sock) == true) {
    sock->ctx = SSL_CTX_new(SSLv23_client_method());
    sock->ssl = SSL_new(sock->ctx);
    if (SSL_set_fd(sock->ssl, mupnp_socket_getid(sock)) == 0) {
      mupnp_socket_close(sock);
      return false;
    }
    if (SSL_connect(sock->ssl) < 1) {
      mupnp_socket_close(sock);
      return false;
    }
  }
```

#### Reachability

Requires the optional native OpenSSL HTTPS API and an on-path attacker.

#### Severity

**Medium** — Requires the optional native OpenSSL HTTPS API and an on-path attacker.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** high
- **Why:** Requires the optional native OpenSSL HTTPS API and an on-path attacker.

Likelihood assessment:
- **Level:** medium
- **Why:** Requires the optional native OpenSSL HTTPS API and an on-path attacker.

#### Remediation

Load trust roots, require peer chain verification and hostname verification, and fail closed.

Tests:
- Load trust roots, require peer chain verification and hostname verification, and fail closed. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

<a id="finding-7"></a>

### [7] Action wrappers leave a dangling Objective-C callback target

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified actual source flow and counterevidence; dynamic exploitation was not tested. |
| Category | use-after-free |
| CWE | CWE-416 |
| Affected lines | wrapper/objc/mUPnP/CGUpnpAction.m:36-44, wrapper/objc/mUPnP/CGUpnpAction.m:53-56, src/mupnp/control/action_ctrl.c:45-68, wrapper/objc/mUPnP/CGUpnpAction.m:17-29 |

#### Summary

Constructing a `CGUpnpAction` installs raw `self` as C userdata and replaces the listener. Its `dealloc` leaves both installed. If the wrapper is released while a hosted C action survives, a later SOAP action dispatch messages the freed Objective-C target.

#### Root Cause

Constructing a `CGUpnpAction` installs raw `self` as C userdata and replaces the listener. Its `dealloc` leaves both installed. If the wrapper is released while a hosted C action survives, a later SOAP action dispatch messages the freed Objective-C target.

**objc register** — `wrapper/objc/mUPnP/CGUpnpAction.m:36-44`

Constructing an action wrapper installs raw self and a C callback.

```objective-c
- (id)initWithCObject:(mUpnpAction*)cobj
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  mupnp_action_setuserdata(cObject, self);
  mupnp_action_setlistener(cObject, cg_upnp_action_listener);
  return self;
}
```

**objc dealloc** — `wrapper/objc/mUPnP/CGUpnpAction.m:53-56`

Wrapper destruction leaves installed C userdata and listener intact.

```objective-c
- (void)dealloc
{
  [super dealloc];
}
```

**action callback** — `src/mupnp/control/action_ctrl.c:45-68`

Incoming hosted action invokes the installed listener.

```c
bool mupnp_action_performlistner(mUpnpAction* action, mUpnpActionRequest* actionReq)
{
  MUPNP_ACTION_LISTNER listener;
  mUpnpActionResponse* actionRes;
  mUpnpHttpRequest* actionReqHttpReq;
  mUpnpHttpResponse* actionResHttpRes;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_action_getlistner(action);
  if (listener == NULL)
    return false;

  actionRes = mupnp_control_action_response_new();

  mupnp_action_setstatuscode(action, MUPNP_STATUS_INVALID_ACTION);
  mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_INVALID_ACTION));

  mupnp_action_clearoutputargumentvalues(action);

  if (listener(action) == true)
    mupnp_control_action_response_setresponse(actionRes, action);
  else
    mupnp_control_soap_response_setfaultresponse(mupnp_control_action_response_getsoapresponse(actionRes), mupnp_action_getstatuscode(action), mupnp_action_getstatusdescription(action));
```

**objc callback** — `wrapper/objc/mUPnP/CGUpnpAction.m:17-29`

Later callback dispatch messages the stored wrapper pointer.

```objective-c
{
  if (!cAction)
    return FALSE;
  CGUpnpAction* objcAction = (CGUpnpAction*)mupnp_action_getuserdata(cAction);
  if (!objcAction)
    return FALSE;
  SEL actionReceived = @selector(actionReceived);
  if (!actionReceived)
    return FALSE;
  if (![objcAction respondsToSelector:actionReceived])
    return FALSE;
  [objcAction performSelector:actionReceived];
  return TRUE;
```

#### Validation

Constructing a `CGUpnpAction` installs raw `self` as C userdata and replaces the listener. Its `dealloc` leaves both installed. If the wrapper is released while a hosted C action survives, a later SOAP action dispatch messages the freed Objective-C target.

Validation method: static source trace

**objc register** — `wrapper/objc/mUPnP/CGUpnpAction.m:36-44`

Constructing an action wrapper installs raw self and a C callback.

```objective-c
- (id)initWithCObject:(mUpnpAction*)cobj
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  mupnp_action_setuserdata(cObject, self);
  mupnp_action_setlistener(cObject, cg_upnp_action_listener);
  return self;
}
```

**objc dealloc** — `wrapper/objc/mUPnP/CGUpnpAction.m:53-56`

Wrapper destruction leaves installed C userdata and listener intact.

```objective-c
- (void)dealloc
{
  [super dealloc];
}
```

**action callback** — `src/mupnp/control/action_ctrl.c:45-68`

Incoming hosted action invokes the installed listener.

```c
bool mupnp_action_performlistner(mUpnpAction* action, mUpnpActionRequest* actionReq)
{
  MUPNP_ACTION_LISTNER listener;
  mUpnpActionResponse* actionRes;
  mUpnpHttpRequest* actionReqHttpReq;
  mUpnpHttpResponse* actionResHttpRes;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_action_getlistner(action);
  if (listener == NULL)
    return false;

  actionRes = mupnp_control_action_response_new();

  mupnp_action_setstatuscode(action, MUPNP_STATUS_INVALID_ACTION);
  mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_INVALID_ACTION));

  mupnp_action_clearoutputargumentvalues(action);

  if (listener(action) == true)
    mupnp_control_action_response_setresponse(actionRes, action);
  else
    mupnp_control_soap_response_setfaultresponse(mupnp_control_action_response_getsoapresponse(actionRes), mupnp_action_getstatuscode(action), mupnp_action_getstatusdescription(action));
```

**objc callback** — `wrapper/objc/mUPnP/CGUpnpAction.m:17-29`

Later callback dispatch messages the stored wrapper pointer.

```objective-c
{
  if (!cAction)
    return FALSE;
  CGUpnpAction* objcAction = (CGUpnpAction*)mupnp_action_getuserdata(cAction);
  if (!objcAction)
    return FALSE;
  SEL actionReceived = @selector(actionReceived);
  if (!actionReceived)
    return FALSE;
  if (![objcAction respondsToSelector:actionReceived])
    return FALSE;
  [objcAction performSelector:actionReceived];
  return TRUE;
```

Limitations:
- No application code or vulnerability-triggering input was executed.

#### Dataflow

objc-register -\> objc-dealloc -\> action-callback -\> objc-callback

**objc register** — `wrapper/objc/mUPnP/CGUpnpAction.m:36-44`

Constructing an action wrapper installs raw self and a C callback.

```objective-c
- (id)initWithCObject:(mUpnpAction*)cobj
{
  if ((self = [super init]) == nil)
    return nil;
  cObject = cobj;
  mupnp_action_setuserdata(cObject, self);
  mupnp_action_setlistener(cObject, cg_upnp_action_listener);
  return self;
}
```

**objc dealloc** — `wrapper/objc/mUPnP/CGUpnpAction.m:53-56`

Wrapper destruction leaves installed C userdata and listener intact.

```objective-c
- (void)dealloc
{
  [super dealloc];
}
```

**action callback** — `src/mupnp/control/action_ctrl.c:45-68`

Incoming hosted action invokes the installed listener.

```c
bool mupnp_action_performlistner(mUpnpAction* action, mUpnpActionRequest* actionReq)
{
  MUPNP_ACTION_LISTNER listener;
  mUpnpActionResponse* actionRes;
  mUpnpHttpRequest* actionReqHttpReq;
  mUpnpHttpResponse* actionResHttpRes;

  mupnp_log_debug_l4("Entering...\n");

  listener = mupnp_action_getlistner(action);
  if (listener == NULL)
    return false;

  actionRes = mupnp_control_action_response_new();

  mupnp_action_setstatuscode(action, MUPNP_STATUS_INVALID_ACTION);
  mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_INVALID_ACTION));

  mupnp_action_clearoutputargumentvalues(action);

  if (listener(action) == true)
    mupnp_control_action_response_setresponse(actionRes, action);
  else
    mupnp_control_soap_response_setfaultresponse(mupnp_control_action_response_getsoapresponse(actionRes), mupnp_action_getstatuscode(action), mupnp_action_getstatusdescription(action));
```

**objc callback** — `wrapper/objc/mUPnP/CGUpnpAction.m:17-29`

Later callback dispatch messages the stored wrapper pointer.

```objective-c
{
  if (!cAction)
    return FALSE;
  CGUpnpAction* objcAction = (CGUpnpAction*)mupnp_action_getuserdata(cAction);
  if (!objcAction)
    return FALSE;
  SEL actionReceived = @selector(actionReceived);
  if (!actionReceived)
    return FALSE;
  if (![objcAction respondsToSelector:actionReceived])
    return FALSE;
  [objcAction performSelector:actionReceived];
  return TRUE;
```

#### Reachability

Requires an embedding application to wrap a hosted action, release the wrapper and keep the native action/listener alive. Retaining the wrapper avoids the immediate dangling condition. The callback accesses the object before checking whether the selector is supported.

#### Severity

**Medium** — Requires an embedding application to wrap a hosted action, release the wrapper and keep the native action/listener alive. Retaining the wrapper avoids the immediate dangling condition. The callback accesses the object before checking whether the selector is supported.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** high
- **Why:** Constructing a `CGUpnpAction` installs raw `self` as C userdata and replaces the listener. Its `dealloc` leaves both installed. If the wrapper is released while a hosted C action survives, a later SOAP action dispatch messages the freed Objective-C target.

Likelihood assessment:
- **Level:** medium
- **Why:** Requires an embedding application to wrap a hosted action, release the wrapper and keep the native action/listener alive. Retaining the wrapper avoids the immediate dangling condition. The callback accesses the object before checking whether the selector is supported.

#### Remediation

Separate inspection wrappers from registration; retain callback targets for the registration lifetime or unregister userdata/listener during synchronized teardown.

Tests:
- Destroy an action wrapper while its hosted C action remains alive and verify callback dispatch never touches the released object.

Preventive controls:
- Centralize and enforce the repaired lifetime or input-limit invariant.

<a id="finding-8"></a>

### [8] Deep XML input can exhaust the stack during tree cleanup

| Field | Value |
| --- | --- |
| Severity | medium |
| Confidence | high |
| Confidence rationale | Parent verified actual source flow and counterevidence; dynamic exploitation was not tested. |
| Category | denial-of-service |
| CWE | CWE-674 |
| Affected lines | src/mupnp/soap/soap_request.c:180-188, src/mupnp/soap/soap_request.c:202-207, src/mupnp/xml/xml_parser_expat.c:95-112, src/mupnp/xml/xml_parser_expat.c:194-199, src/mupnp/xml/xml_node.c:48-59 |

#### Summary

The Expat callbacks build an arbitrarily deep child tree without a depth budget. XML parse failure deletes that tree through recursive child-list destruction. Network SOAP input reaches parsing before service/action validation, so a deeply nested malformed document can exhaust the native stack.

#### Root Cause

The Expat callbacks build an arbitrarily deep child tree without a depth budget. XML parse failure deletes that tree through recursive child-list destruction. Network SOAP input reaches parsing before service/action validation, so a deeply nested malformed document can exhaust the native stack.

**soap input** — `src/mupnp/soap/soap_request.c:180-188`

HTTP content and explicit framing length pass together into SOAP parsing.

```c
  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
```

**soap parse** — `src/mupnp/soap/soap_request.c:202-207`

SOAP forwards bytes and length to the active XML backend.

```c
  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);
```

**expat depth** — `src/mupnp/xml/xml_parser_expat.c:95-112`

Each nested element allocates and links a child without depth accounting.

```c
  expatData = (mUpnpExpatData*)userData;

  node = mupnp_xml_node_new();
  mupnp_xml_node_setname(node, (char*)el);

  for (n = 0; attr[n]; n += 2)
    mupnp_xml_node_setattribute(node, (char*)attr[n], (char*)attr[n + 1]);

  if (expatData->rootNode != NULL) {
    if (expatData->currNode != NULL)
      mupnp_xml_node_addchildnode(expatData->currNode, node);
    else
      mupnp_xml_node_addchildnode(expatData->rootNode, node);
  }
  else
    expatData->rootNode = node;

  expatData->currNode = node;
```

**expat error** — `src/mupnp/xml/xml_parser_expat.c:194-199`

Parse failure deletes the partially constructed tree.

```c
  parser->parseResult = XML_Parse(p, data, len, 1);
  XML_ParserFree(p);

  if (parser->parseResult == 0 /*XML_STATUS_ERROR*/) {
    if (expatData.rootNode != NULL)
      mupnp_xml_node_delete(expatData.rootNode);
```

**xml delete** — `src/mupnp/xml/xml_node.c:48-59`

Node deletion recursively deletes child lists.

```c
void mupnp_xml_node_delete(mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)node);
  mupnp_string_delete(node->name);
  mupnp_string_delete(node->value);
  mupnp_xml_attributelist_delete(node->attrList);
  mupnp_xml_nodelist_delete(node->nodeList);
  if (node->userDataDestructorFunc != NULL)
    node->userDataDestructorFunc(node->userData);
  free(node);
```

#### Validation

The Expat callbacks build an arbitrarily deep child tree without a depth budget. XML parse failure deletes that tree through recursive child-list destruction. Network SOAP input reaches parsing before service/action validation, so a deeply nested malformed document can exhaust the native stack.

Validation method: static source trace

**soap input** — `src/mupnp/soap/soap_request.c:180-188`

HTTP content and explicit framing length pass together into SOAP parsing.

```c
  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
```

**soap parse** — `src/mupnp/soap/soap_request.c:202-207`

SOAP forwards bytes and length to the active XML backend.

```c
  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);
```

**expat depth** — `src/mupnp/xml/xml_parser_expat.c:95-112`

Each nested element allocates and links a child without depth accounting.

```c
  expatData = (mUpnpExpatData*)userData;

  node = mupnp_xml_node_new();
  mupnp_xml_node_setname(node, (char*)el);

  for (n = 0; attr[n]; n += 2)
    mupnp_xml_node_setattribute(node, (char*)attr[n], (char*)attr[n + 1]);

  if (expatData->rootNode != NULL) {
    if (expatData->currNode != NULL)
      mupnp_xml_node_addchildnode(expatData->currNode, node);
    else
      mupnp_xml_node_addchildnode(expatData->rootNode, node);
  }
  else
    expatData->rootNode = node;

  expatData->currNode = node;
```

**expat error** — `src/mupnp/xml/xml_parser_expat.c:194-199`

Parse failure deletes the partially constructed tree.

```c
  parser->parseResult = XML_Parse(p, data, len, 1);
  XML_ParserFree(p);

  if (parser->parseResult == 0 /*XML_STATUS_ERROR*/) {
    if (expatData.rootNode != NULL)
      mupnp_xml_node_delete(expatData.rootNode);
```

**xml delete** — `src/mupnp/xml/xml_node.c:48-59`

Node deletion recursively deletes child lists.

```c
void mupnp_xml_node_delete(mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)node);
  mupnp_string_delete(node->name);
  mupnp_string_delete(node->value);
  mupnp_xml_attributelist_delete(node->attrList);
  mupnp_xml_nodelist_delete(node->nodeList);
  if (node->userDataDestructorFunc != NULL)
    node->userDataDestructorFunc(node->userData);
  free(node);
```

Limitations:
- No application code or vulnerability-triggering input was executed.

#### Dataflow

soap-input -\> soap-parse -\> expat-depth -\> expat-error -\> xml-delete

**soap input** — `src/mupnp/soap/soap_request.c:180-188`

HTTP content and explicit framing length pass together into SOAP parsing.

```c
  content = mupnp_http_request_getcontent(httpReq);
  contentLen = mupnp_http_request_getcontentlength(httpReq);

  if (content == NULL || contentLen <= 0)
    return false;

  mupnp_log_debug_l4("Leaving...\n");

  return mupnp_soap_request_parsemessage(soapReq, content, contentLen);
```

**soap parse** — `src/mupnp/soap/soap_request.c:202-207`

SOAP forwards bytes and length to the active XML backend.

```c
  if (msgLen <= 0)
    return false;

  xmlParser = mupnp_xml_parser_new();
  parseRet = mupnp_xml_parse(xmlParser, soapReq->rootNodeList, msg, msgLen);
  mupnp_xml_parser_delete(xmlParser);
```

**expat depth** — `src/mupnp/xml/xml_parser_expat.c:95-112`

Each nested element allocates and links a child without depth accounting.

```c
  expatData = (mUpnpExpatData*)userData;

  node = mupnp_xml_node_new();
  mupnp_xml_node_setname(node, (char*)el);

  for (n = 0; attr[n]; n += 2)
    mupnp_xml_node_setattribute(node, (char*)attr[n], (char*)attr[n + 1]);

  if (expatData->rootNode != NULL) {
    if (expatData->currNode != NULL)
      mupnp_xml_node_addchildnode(expatData->currNode, node);
    else
      mupnp_xml_node_addchildnode(expatData->rootNode, node);
  }
  else
    expatData->rootNode = node;

  expatData->currNode = node;
```

**expat error** — `src/mupnp/xml/xml_parser_expat.c:194-199`

Parse failure deletes the partially constructed tree.

```c
  parser->parseResult = XML_Parse(p, data, len, 1);
  XML_ParserFree(p);

  if (parser->parseResult == 0 /*XML_STATUS_ERROR*/) {
    if (expatData.rootNode != NULL)
      mupnp_xml_node_delete(expatData.rootNode);
```

**xml delete** — `src/mupnp/xml/xml_node.c:48-59`

Node deletion recursively deletes child lists.

```c
void mupnp_xml_node_delete(mUpnpXmlNode* node)
{
  mupnp_log_debug_l4("Entering...\n");

  mupnp_list_remove((mUpnpList*)node);
  mupnp_string_delete(node->name);
  mupnp_string_delete(node->value);
  mupnp_xml_attributelist_delete(node->attrList);
  mupnp_xml_nodelist_delete(node->nodeList);
  if (node->userDataDestructorFunc != NULL)
    node->userDataDestructorFunc(node->userData);
  free(node);
```

#### Reachability

Applies to the Expat backend; libxml2 has separate parser limits. The exact exhaustion threshold depends on native stack size and dependency behavior. No runtime reproduction was performed.

#### Severity

**Medium** — Applies to the Expat backend; libxml2 has separate parser limits. The exact exhaustion threshold depends on native stack size and dependency behavior. No runtime reproduction was performed.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** medium
- **Why:** The Expat callbacks build an arbitrarily deep child tree without a depth budget. XML parse failure deletes that tree through recursive child-list destruction. Network SOAP input reaches parsing before service/action validation, so a deeply nested malformed document can exhaust the native stack.

Likelihood assessment:
- **Level:** high
- **Why:** Applies to the Expat backend; libxml2 has separate parser limits. The exact exhaustion threshold depends on native stack size and dependency behavior. No runtime reproduction was performed.

#### Remediation

Enforce a conservative element-depth and node budget during parsing, abort safely on excess, and make tree destruction iterative.

Tests:
- Verify excessive nesting fails safely on both malformed and valid documents and ordinary SOAP still parses.

Preventive controls:
- Centralize and enforce the repaired lifetime or input-limit invariant.

<a id="finding-9"></a>

### [9] Unbounded M-SEARCH MX blocks the SSDP worker

| Field | Value |
| --- | --- |
| Severity | low |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | denial-of-service |
| CWE | CWE-400 |
| Affected lines | src/mupnp/device_ssdp_server.c:84-107, src/mupnp/util/time.c:64-74 |

#### Summary

Digit validation accepts excessive MX. Delay runs synchronously on sole SSDP worker; duplicate filtering does not cap first delay. Windows and Unix accepting long usleep durations can stop discovery processing for extended periods; platform applicability varies.

#### Root Cause

Digit validation accepts excessive MX. Delay runs synchronously on sole SSDP worker; duplicate filtering does not cap first delay. Windows and Unix accepting long usleep durations can stop discovery processing for extended periods; platform applicability varies.

**mx sleep** — `src/mupnp/device_ssdp_server.c:84-107`

Digits are accepted without a range cap and MX then controls a synchronous delay.

```c
    if (ssdpMXString == NULL || mupnp_strlen(ssdpMXString) == 0)
      /* return if the MX value does not exist or is empty */
      return;
    /* check if MX value is not an integer */
    for (n = 0; n < strlen(ssdpMXString); n++) {
      if (isdigit(ssdpMXString[n]) == 0)
        /* MX value contains a non-digit so is invalid */
        return;
    }

    /****************************************
     * check ST header and if empty return
     ***************************************/
    if (mupnp_strlen(ssdpST) <= 0)
      return;

    /* Check if we have received this search recently
     * and ignore duplicates. */
    if (filter_duplicate_m_search(ssdpPkt))
      return;

    ssdpMX = mupnp_ssdp_packet_getmx(ssdpPkt);
    mupnp_log_debug("Sleeping for a while... (MX:%d)\n", ssdpMX);
    mupnp_waitrandom((ssdpMX * 1000) / 4);
```

**wait random** — `src/mupnp/util/time.c:64-74`

The random delay reaches the platform sleep routine.

```c
void mupnp_waitrandom(mUpnpTime mtime)
{
  double factor;
  long waitTime;

  mupnp_log_debug_l4("Entering...\n");

  factor = (double)rand() / (double)RAND_MAX;
  waitTime = (long)((double)mtime * factor);
  mupnp_wait(waitTime);

```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. Digit validation accepts excessive MX. Delay runs synchronously on sole SSDP worker; duplicate filtering does not cap first delay. Windows and Unix accepting long usleep durations can stop discovery processing for extended periods; platform applicability varies.

Validation method: static source trace

**mx sleep** — `src/mupnp/device_ssdp_server.c:84-107`

Digits are accepted without a range cap and MX then controls a synchronous delay.

```c
    if (ssdpMXString == NULL || mupnp_strlen(ssdpMXString) == 0)
      /* return if the MX value does not exist or is empty */
      return;
    /* check if MX value is not an integer */
    for (n = 0; n < strlen(ssdpMXString); n++) {
      if (isdigit(ssdpMXString[n]) == 0)
        /* MX value contains a non-digit so is invalid */
        return;
    }

    /****************************************
     * check ST header and if empty return
     ***************************************/
    if (mupnp_strlen(ssdpST) <= 0)
      return;

    /* Check if we have received this search recently
     * and ignore duplicates. */
    if (filter_duplicate_m_search(ssdpPkt))
      return;

    ssdpMX = mupnp_ssdp_packet_getmx(ssdpPkt);
    mupnp_log_debug("Sleeping for a while... (MX:%d)\n", ssdpMX);
    mupnp_waitrandom((ssdpMX * 1000) / 4);
```

**wait random** — `src/mupnp/util/time.c:64-74`

The random delay reaches the platform sleep routine.

```c
void mupnp_waitrandom(mUpnpTime mtime)
{
  double factor;
  long waitTime;

  mupnp_log_debug_l4("Entering...\n");

  factor = (double)rand() / (double)RAND_MAX;
  waitTime = (long)((double)mtime * factor);
  mupnp_wait(waitTime);

```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

mx-sleep -\> wait-random

**mx sleep** — `src/mupnp/device_ssdp_server.c:84-107`

Digits are accepted without a range cap and MX then controls a synchronous delay.

```c
    if (ssdpMXString == NULL || mupnp_strlen(ssdpMXString) == 0)
      /* return if the MX value does not exist or is empty */
      return;
    /* check if MX value is not an integer */
    for (n = 0; n < strlen(ssdpMXString); n++) {
      if (isdigit(ssdpMXString[n]) == 0)
        /* MX value contains a non-digit so is invalid */
        return;
    }

    /****************************************
     * check ST header and if empty return
     ***************************************/
    if (mupnp_strlen(ssdpST) <= 0)
      return;

    /* Check if we have received this search recently
     * and ignore duplicates. */
    if (filter_duplicate_m_search(ssdpPkt))
      return;

    ssdpMX = mupnp_ssdp_packet_getmx(ssdpPkt);
    mupnp_log_debug("Sleeping for a while... (MX:%d)\n", ssdpMX);
    mupnp_waitrandom((ssdpMX * 1000) / 4);
```

**wait random** — `src/mupnp/util/time.c:64-74`

The random delay reaches the platform sleep routine.

```c
void mupnp_waitrandom(mUpnpTime mtime)
{
  double factor;
  long waitTime;

  mupnp_log_debug_l4("Entering...\n");

  factor = (double)rand() / (double)RAND_MAX;
  waitTime = (long)((double)mtime * factor);
  mupnp_wait(waitTime);

```

#### Reachability

The discovery subsystem stalls, with platform-dependent long-sleep behavior.

#### Severity

**Low** — The discovery subsystem stalls, with platform-dependent long-sleep behavior.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** low
- **Why:** The discovery subsystem stalls, with platform-dependent long-sleep behavior.

Likelihood assessment:
- **Level:** high
- **Why:** The discovery subsystem stalls, with platform-dependent long-sleep behavior.

#### Remediation

Range-check and cap MX and schedule responses without blocking reception.

Tests:
- Range-check and cap MX and schedule responses without blocking reception. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

<a id="finding-10"></a>

### [10] Test presentation handler sends uninitialized stack contents

| Field | Value |
| --- | --- |
| Severity | low |
| Confidence | high |
| Confidence rationale | Parent verified actual source flow and counterevidence; dynamic exploitation was not tested. |
| Category | information-disclosure |
| CWE | CWE-200 |
| Affected lines | test/TestDevice.c:309-313, test/DeviceTest.cpp:41-44, test/TestDevice.c:208-224, test/TestDevice.c:273-278 |

#### Summary

The network test device's presentation handler declares a stack array whose only initialization is commented out, then copies and measures it as a C string before sending it. A reachable peer during test execution can receive residual stack bytes or cause an out-of-bounds read.

#### Root Cause

The network test device's presentation handler declares a stack array whose only initialization is commented out, then copies and measures it as a C string before sending it. A reachable peer during test execution can receive residual stack bytes or cause an out-of-bounds read.

**test register** — `test/TestDevice.c:309-313`

The test device registers this HTTP listener.

```c
  mupnp_device_setactionlistener(testDev, upnp_test_actionreceived);
  mupnp_device_setquerylistener(testDev, upnp_test_queryreceived);
  mupnp_device_sethttplistener(testDev, upnp_test_device_httprequestrecieved);

  return testDev;
```

**test start** — `test/DeviceTest.cpp:41-44`

The test starts the device, exposing the listener for the test duration.

```c
  BOOST_REQUIRE(mupnp_device_start(testDev));

  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
```

**test buffer** — `test/TestDevice.c:208-224`

The presentation handler declares an uninitialized stack array.

```c
void upnp_test_device_httprequestrecieved(mUpnpHttpRequest* httpReq)
{
  // mUpnpTime currTime;
  // mUpnpDevice* dev;
  char* uri;
  char content[2048];
  // char sysTimeStr[SYSTEM_TIME_BUF_LEN];
  // char serverName[MUPNP_SEVERNAME_MAXLEN];
  mUpnpHttpResponse* httpRes;

  // dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  uri = mupnp_http_request_geturi(httpReq);
  if (strcmp(uri, "/presentation") != 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }
```

**test send** — `test/TestDevice.c:273-278`

The uninitialized array is copied as a C string and sent to the requester.

```c
  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, content);
  mupnp_http_response_setcontenttype(httpRes, "text/html");
  mupnp_http_response_setcontentlength(httpRes, strlen(content));
  mupnp_http_request_postresponse(httpReq, httpRes);
```

#### Validation

The network test device's presentation handler declares a stack array whose only initialization is commented out, then copies and measures it as a C string before sending it. A reachable peer during test execution can receive residual stack bytes or cause an out-of-bounds read.

Validation method: static source trace

**test register** — `test/TestDevice.c:309-313`

The test device registers this HTTP listener.

```c
  mupnp_device_setactionlistener(testDev, upnp_test_actionreceived);
  mupnp_device_setquerylistener(testDev, upnp_test_queryreceived);
  mupnp_device_sethttplistener(testDev, upnp_test_device_httprequestrecieved);

  return testDev;
```

**test start** — `test/DeviceTest.cpp:41-44`

The test starts the device, exposing the listener for the test duration.

```c
  BOOST_REQUIRE(mupnp_device_start(testDev));

  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
```

**test buffer** — `test/TestDevice.c:208-224`

The presentation handler declares an uninitialized stack array.

```c
void upnp_test_device_httprequestrecieved(mUpnpHttpRequest* httpReq)
{
  // mUpnpTime currTime;
  // mUpnpDevice* dev;
  char* uri;
  char content[2048];
  // char sysTimeStr[SYSTEM_TIME_BUF_LEN];
  // char serverName[MUPNP_SEVERNAME_MAXLEN];
  mUpnpHttpResponse* httpRes;

  // dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  uri = mupnp_http_request_geturi(httpReq);
  if (strcmp(uri, "/presentation") != 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }
```

**test send** — `test/TestDevice.c:273-278`

The uninitialized array is copied as a C string and sent to the requester.

```c
  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, content);
  mupnp_http_response_setcontenttype(httpRes, "text/html");
  mupnp_http_response_setcontentlength(httpRes, strlen(content));
  mupnp_http_request_postresponse(httpReq, httpRes);
```

Limitations:
- No application code or vulnerability-triggering input was executed.

#### Dataflow

test-register -\> test-start -\> test-buffer -\> test-send

**test register** — `test/TestDevice.c:309-313`

The test device registers this HTTP listener.

```c
  mupnp_device_setactionlistener(testDev, upnp_test_actionreceived);
  mupnp_device_setquerylistener(testDev, upnp_test_queryreceived);
  mupnp_device_sethttplistener(testDev, upnp_test_device_httprequestrecieved);

  return testDev;
```

**test start** — `test/DeviceTest.cpp:41-44`

The test starts the device, exposing the listener for the test duration.

```c
  BOOST_REQUIRE(mupnp_device_start(testDev));

  BOOST_REQUIRE(mupnp_device_stop(testDev));
  mupnp_device_delete(testDev);
```

**test buffer** — `test/TestDevice.c:208-224`

The presentation handler declares an uninitialized stack array.

```c
void upnp_test_device_httprequestrecieved(mUpnpHttpRequest* httpReq)
{
  // mUpnpTime currTime;
  // mUpnpDevice* dev;
  char* uri;
  char content[2048];
  // char sysTimeStr[SYSTEM_TIME_BUF_LEN];
  // char serverName[MUPNP_SEVERNAME_MAXLEN];
  mUpnpHttpResponse* httpRes;

  // dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  uri = mupnp_http_request_geturi(httpReq);
  if (strcmp(uri, "/presentation") != 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }
```

**test send** — `test/TestDevice.c:273-278`

The uninitialized array is copied as a C string and sent to the requester.

```c
  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, content);
  mupnp_http_response_setcontenttype(httpRes, "text/html");
  mupnp_http_response_setcontentlength(httpRes, strlen(content));
  mupnp_http_request_postresponse(httpReq, httpRes);
```

#### Reachability

Test-only; CMake tests default OFF, CI explicitly enables them. The listener's exposure window and confidential stack contents depend on the test environment; no concrete secret was observed.

#### Severity

**Low** — Test-only; CMake tests default OFF, CI explicitly enables them. The listener's exposure window and confidential stack contents depend on the test environment; no concrete secret was observed.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** medium
- **Why:** The network test device's presentation handler declares a stack array whose only initialization is commented out, then copies and measures it as a C string before sending it. A reachable peer during test execution can receive residual stack bytes or cause an out-of-bounds read.

Likelihood assessment:
- **Level:** low
- **Why:** Test-only; CMake tests default OFF, CI explicitly enables them. The listener's exposure window and confidential stack contents depend on the test environment; no concrete secret was observed.

#### Remediation

Send an initialized literal with an explicit known length; use loopback for network test listeners when external reachability is unnecessary.

Tests:
- Verify the test presentation response is exactly the fixed initialized bytes and uses the known length.

Preventive controls:
- Centralize and enforce the repaired lifetime or input-limit invariant.

<a id="finding-11"></a>

### [11] Media directory dump follows container cycles without a bound

| Field | Value |
| --- | --- |
| Severity | low |
| Confidence | high |
| Confidence rationale | Parent verified actual source flow and counterevidence; dynamic exploitation was not tested. |
| Category | denial-of-service |
| CWE | CWE-674 |
| Affected lines | examples/upnpavdump/upnpavdump.c:48-58, examples/upnpavdump/upnpavdump.c:60-70, examples/upnpavchk/upnpavchk.c:61-70, examples/upnpavdump/macosx/xcode30/upnpavdump.m:46-49 |

#### Summary

The media dump utilities recursively browse every container ID supplied by a successful remote DIDL response. No visited-ID set or depth/request budget stops a cyclic or excessively deep hierarchy, retaining native frames and XML trees until stack or memory exhaustion.

#### Root Cause

The media dump utilities recursively browse every container ID supplied by a successful remote DIDL response. No visited-ID set or depth/request budget stops a cyclic or excessively deep hierarchy, retaining native frames and XML trees until stack or memory exhaustion.

**media response** — `examples/upnpavdump/upnpavdump.c:48-58`

A successful remote action supplies the XML containing the next container IDs.

```c

  if (!mupnp_action_post(browseAction))
    return;

  resultXml = mupnp_action_getargumentvaluebyname(browseAction, "Result");
  if (mupnp_strlen(resultXml) <= 0)
    return;

  rootNode = mupnp_xml_nodelist_new();
  xmlParser = mupnp_xml_parser_new();
  if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml))) {
```

**media recursion** — `examples/upnpavdump/upnpavdump.c:60-70`

Peer-returned container IDs drive recursive Browse traversal without a cycle or depth guard.

```c
    if (didlNode) {
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          print_content_directory(browseAction, (indent + 1), id);
        }
```

**media chk recursion** — `examples/upnpavchk/upnpavchk.c:61-70`

The sibling traversal has the same unbounded container recursion.

```c
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          PrintContentDirectory(browseAction, (indent + 1), id);
        }
```

**media objc recursion** — `examples/upnpavdump/macosx/xcode30/upnpavdump.m:46-49`

The macOS sibling also recurses using peer container IDs.

```objective-c
    if ([[contentNode name] isEqualToString:@"container"]) {
      NSLog(@"%@  [%@] %@", indentStr, objId, title);
      PrintContentDirectory(browseAction, (indent + 1), objId);
    }
```

#### Validation

The media dump utilities recursively browse every container ID supplied by a successful remote DIDL response. No visited-ID set or depth/request budget stops a cyclic or excessively deep hierarchy, retaining native frames and XML trees until stack or memory exhaustion.

Validation method: static source trace

**media response** — `examples/upnpavdump/upnpavdump.c:48-58`

A successful remote action supplies the XML containing the next container IDs.

```c

  if (!mupnp_action_post(browseAction))
    return;

  resultXml = mupnp_action_getargumentvaluebyname(browseAction, "Result");
  if (mupnp_strlen(resultXml) <= 0)
    return;

  rootNode = mupnp_xml_nodelist_new();
  xmlParser = mupnp_xml_parser_new();
  if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml))) {
```

**media recursion** — `examples/upnpavdump/upnpavdump.c:60-70`

Peer-returned container IDs drive recursive Browse traversal without a cycle or depth guard.

```c
    if (didlNode) {
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          print_content_directory(browseAction, (indent + 1), id);
        }
```

**media chk recursion** — `examples/upnpavchk/upnpavchk.c:61-70`

The sibling traversal has the same unbounded container recursion.

```c
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          PrintContentDirectory(browseAction, (indent + 1), id);
        }
```

**media objc recursion** — `examples/upnpavdump/macosx/xcode30/upnpavdump.m:46-49`

The macOS sibling also recurses using peer container IDs.

```objective-c
    if ([[contentNode name] isEqualToString:@"container"]) {
      NSLog(@"%@  [%@] %@", indentStr, objId, title);
      PrintContentDirectory(browseAction, (indent + 1), objId);
    }
```

Limitations:
- No application code or vulnerability-triggering input was executed.

#### Dataflow

media-response -\> media-recursion -\> media-chk-recursion -\> media-objc-recursion

**media response** — `examples/upnpavdump/upnpavdump.c:48-58`

A successful remote action supplies the XML containing the next container IDs.

```c

  if (!mupnp_action_post(browseAction))
    return;

  resultXml = mupnp_action_getargumentvaluebyname(browseAction, "Result");
  if (mupnp_strlen(resultXml) <= 0)
    return;

  rootNode = mupnp_xml_nodelist_new();
  xmlParser = mupnp_xml_parser_new();
  if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml))) {
```

**media recursion** — `examples/upnpavdump/upnpavdump.c:60-70`

Peer-returned container IDs drive recursive Browse traversal without a cycle or depth guard.

```c
    if (didlNode) {
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          print_content_directory(browseAction, (indent + 1), id);
        }
```

**media chk recursion** — `examples/upnpavchk/upnpavchk.c:61-70`

The sibling traversal has the same unbounded container recursion.

```c
      for (cnode = mupnp_xml_node_getchildnodes(didlNode); cnode; cnode = mupnp_xml_node_next(cnode)) {
        id = mupnp_xml_node_getattributevalue(cnode, "id");
        title = mupnp_xml_node_getchildnodevalue(cnode, "dc:title");
        if (mupnp_xml_node_isname(cnode, "container")) {
          printf(" %s[%s]%s\n",
              indentStr,
              id,
              ((0 < mupnp_strlen(title)) ? title : ""));
          PrintContentDirectory(browseAction, (indent + 1), id);
        }
```

**media objc recursion** — `examples/upnpavdump/macosx/xcode30/upnpavdump.m:46-49`

The macOS sibling also recurses using peer container IDs.

```objective-c
    if ([[contentNode name] isEqualToString:@"container"]) {
      NSLog(@"%@  [%@] %@", indentStr, objId, title);
      PrintContentDirectory(browseAction, (indent + 1), objId);
    }
```

#### Reachability

Availability impact is confined to a user-invoked dump utility. The primary C upnpavdump target is in default example builds; other sibling targets have separate/legacy build routes. Indentation is bounded, which prevents a distinct buffer overflow but does not bound recursion.

#### Severity

**Low** — Availability impact is confined to a user-invoked dump utility. The primary C upnpavdump target is in default example builds; other sibling targets have separate/legacy build routes. Indentation is bounded, which prevents a distinct buffer overflow but does not bound recursion.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** low
- **Why:** The media dump utilities recursively browse every container ID supplied by a successful remote DIDL response. No visited-ID set or depth/request budget stops a cyclic or excessively deep hierarchy, retaining native frames and XML trees until stack or memory exhaustion.

Likelihood assessment:
- **Level:** high
- **Why:** Availability impact is confined to a user-invoked dump utility. The primary C upnpavdump target is in default example builds; other sibling targets have separate/legacy build routes. Indentation is bounded, which prevents a distinct buffer overflow but does not bound recursion.

#### Remediation

Use iterative traversal with a per-device visited-ID set and explicit depth, response-count and content budgets.

Tests:
- Verify cyclic and overly deep container responses terminate within the configured budget.

Preventive controls:
- Centralize and enforce the repaired lifetime or input-limit invariant.

<a id="finding-12"></a>

### [12] Malformed SSDP packets leak tokenizers without bound

| Field | Value |
| --- | --- |
| Severity | low |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | resource-leak |
| CWE | CWE-401 |
| Affected lines | src/mupnp/ssdp/httpmu_socket.c:48-62, src/mupnp/ssdp/ssdp_packet.c:111-115 |

#### Summary

Nonempty malformed datagram produces no tokenizer token. New tokenizer is abandoned on early return before message validation. Packet cleanup does not own the local tokenizer. Sustained unauthenticated UDP traffic leaks heap.

#### Root Cause

Nonempty malformed datagram produces no tokenizer token. New tokenizer is abandoned on early return before message validation. Packet cleanup does not own the local tokenizer. Sustained unauthenticated UDP traffic leaks heap.

**ssdp before validation** — `src/mupnp/ssdp/httpmu_socket.c:48-62`

Headers are parsed from received datagram bytes before listener validation.

```c
  dgmPkt = mupnp_ssdp_packet_getdatagrampacket(ssdpPkt);
  recvLen = mupnp_socket_recv(sock, dgmPkt);

  if (recvLen <= 0)
    return recvLen;

  ssdpData = mupnp_socket_datagram_packet_getdata(dgmPkt);

  /* set header information to the packets headerlist,
           this will leave only the request line in the datagram packet
           which is need to verify the message */
  mupnp_ssdp_packet_setheader(ssdpPkt, ssdpData);

  mupnp_log_debug_l4("Leaving...\n");

```

**ssdp leak** — `src/mupnp/ssdp/ssdp_packet.c:111-115`

The allocated local tokenizer is abandoned when no tokens are present.

```c
  ssdpTok = mupnp_string_tokenizer_new(ssdpMsg, MUPNP_HTTP_CRLF);

  /**** skip the first line ****/
  if (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == false)
    return;
```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. Nonempty malformed datagram produces no tokenizer token. New tokenizer is abandoned on early return before message validation. Packet cleanup does not own the local tokenizer. Sustained unauthenticated UDP traffic leaks heap.

Validation method: static source trace

**ssdp before validation** — `src/mupnp/ssdp/httpmu_socket.c:48-62`

Headers are parsed from received datagram bytes before listener validation.

```c
  dgmPkt = mupnp_ssdp_packet_getdatagrampacket(ssdpPkt);
  recvLen = mupnp_socket_recv(sock, dgmPkt);

  if (recvLen <= 0)
    return recvLen;

  ssdpData = mupnp_socket_datagram_packet_getdata(dgmPkt);

  /* set header information to the packets headerlist,
           this will leave only the request line in the datagram packet
           which is need to verify the message */
  mupnp_ssdp_packet_setheader(ssdpPkt, ssdpData);

  mupnp_log_debug_l4("Leaving...\n");

```

**ssdp leak** — `src/mupnp/ssdp/ssdp_packet.c:111-115`

The allocated local tokenizer is abandoned when no tokens are present.

```c
  ssdpTok = mupnp_string_tokenizer_new(ssdpMsg, MUPNP_HTTP_CRLF);

  /**** skip the first line ****/
  if (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == false)
    return;
```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

ssdp-before-validation -\> ssdp-leak

**ssdp before validation** — `src/mupnp/ssdp/httpmu_socket.c:48-62`

Headers are parsed from received datagram bytes before listener validation.

```c
  dgmPkt = mupnp_ssdp_packet_getdatagrampacket(ssdpPkt);
  recvLen = mupnp_socket_recv(sock, dgmPkt);

  if (recvLen <= 0)
    return recvLen;

  ssdpData = mupnp_socket_datagram_packet_getdata(dgmPkt);

  /* set header information to the packets headerlist,
           this will leave only the request line in the datagram packet
           which is need to verify the message */
  mupnp_ssdp_packet_setheader(ssdpPkt, ssdpData);

  mupnp_log_debug_l4("Leaving...\n");

```

**ssdp leak** — `src/mupnp/ssdp/ssdp_packet.c:111-115`

The allocated local tokenizer is abandoned when no tokens are present.

```c
  ssdpTok = mupnp_string_tokenizer_new(ssdpMsg, MUPNP_HTTP_CRLF);

  /**** skip the first line ****/
  if (mupnp_string_tokenizer_hasmoretoken(ssdpTok) == false)
    return;
```

#### Reachability

Sustained traffic is required to turn a small per-packet leak into material memory pressure.

#### Severity

**Low** — Sustained traffic is required to turn a small per-packet leak into material memory pressure.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** low
- **Why:** Sustained traffic is required to turn a small per-packet leak into material memory pressure.

Likelihood assessment:
- **Level:** high
- **Why:** Sustained traffic is required to turn a small per-packet leak into material memory pressure.

#### Remediation

Destroy tokenizer on all exit paths.

Tests:
- Destroy tokenizer on all exit paths. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

<a id="finding-13"></a>

### [13] A zero-length UDP packet permanently stops SSDP reception

| Field | Value |
| --- | --- |
| Severity | low |
| Confidence | high |
| Confidence rationale | Parent verified the source control, callers, signed return semantics and counterevidence; no runtime reproduction. |
| Category | denial-of-service |
| CWE | CWE-754 |
| Affected lines | src/mupnp/net/socket.c:966-977, src/mupnp/ssdp/ssdp_server.c:148-153, src/mupnp/ssdp/ssdp_response_server.c:150-157 |

#### Summary

Empty datagram yields zero from recvfrom; socket_recv/HTTPMU/HTTPU propagate zero. Both multicast and response workers exit before validation. recvThread remains non-NULL; no automatic restart. Requires UDP reachability to targeted listener.

#### Root Cause

Empty datagram yields zero from recvfrom; socket_recv/HTTPMU/HTTPU propagate zero. Both multicast and response workers exit before validation. recvThread remains non-NULL; no automatic restart. Requires UDP reachability to targeted listener.

**udp recv** — `src/mupnp/net/socket.c:966-977`

A valid empty UDP datagram produces zero, treated identically to error.

```c
  struct sockaddr_storage from;
  socklen_t fromLen = sizeof(from);
  recvLen = recvfrom(sock->id, recvBuf, sizeof(recvBuf) - 1, 0, (struct sockaddr*)&from, &fromLen);
#endif

  mupnp_log_debug_l4("Entering...\n");

  if (recvLen <= 0)
    return 0;

  recvBuf[recvLen] = '\0';
  mupnp_socket_datagram_packet_setdata(dgmPkt, recvBuf);
```

**ssdp zero** — `src/mupnp/ssdp/ssdp_server.c:148-153`

Zero-length receive causes the multicast worker to stop.

```c
  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpmu_socket_recv(server->httpmuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
```

**ssdp response zero** — `src/mupnp/ssdp/ssdp_response_server.c:150-157`

The unicast response worker also exits on an empty datagram.

```c
  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpu_socket_recv(server->httpuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_packet_print(ssdpPkt);

    mupnp_ssdpresponse_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
```

#### Validation

Parent source review confirms the input-to-control-to-outcome path. Empty datagram yields zero from recvfrom; socket_recv/HTTPMU/HTTPU propagate zero. Both multicast and response workers exit before validation. recvThread remains non-NULL; no automatic restart. Requires UDP reachability to targeted listener.

Validation method: static source trace

**udp recv** — `src/mupnp/net/socket.c:966-977`

A valid empty UDP datagram produces zero, treated identically to error.

```c
  struct sockaddr_storage from;
  socklen_t fromLen = sizeof(from);
  recvLen = recvfrom(sock->id, recvBuf, sizeof(recvBuf) - 1, 0, (struct sockaddr*)&from, &fromLen);
#endif

  mupnp_log_debug_l4("Entering...\n");

  if (recvLen <= 0)
    return 0;

  recvBuf[recvLen] = '\0';
  mupnp_socket_datagram_packet_setdata(dgmPkt, recvBuf);
```

**ssdp zero** — `src/mupnp/ssdp/ssdp_server.c:148-153`

Zero-length receive causes the multicast worker to stop.

```c
  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpmu_socket_recv(server->httpmuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
```

**ssdp response zero** — `src/mupnp/ssdp/ssdp_response_server.c:150-157`

The unicast response worker also exits on an empty datagram.

```c
  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpu_socket_recv(server->httpuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_packet_print(ssdpPkt);

    mupnp_ssdpresponse_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
```

Limitations:
- No application code, exploit input or live network test was executed.

#### Dataflow

udp-recv -\> ssdp-zero -\> ssdp-response-zero

**udp recv** — `src/mupnp/net/socket.c:966-977`

A valid empty UDP datagram produces zero, treated identically to error.

```c
  struct sockaddr_storage from;
  socklen_t fromLen = sizeof(from);
  recvLen = recvfrom(sock->id, recvBuf, sizeof(recvBuf) - 1, 0, (struct sockaddr*)&from, &fromLen);
#endif

  mupnp_log_debug_l4("Entering...\n");

  if (recvLen <= 0)
    return 0;

  recvBuf[recvLen] = '\0';
  mupnp_socket_datagram_packet_setdata(dgmPkt, recvBuf);
```

**ssdp zero** — `src/mupnp/ssdp/ssdp_server.c:148-153`

Zero-length receive causes the multicast worker to stop.

```c
  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpmu_socket_recv(server->httpmuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
```

**ssdp response zero** — `src/mupnp/ssdp/ssdp_response_server.c:150-157`

The unicast response worker also exits on an empty datagram.

```c
  while (mupnp_thread_isrunnable(thread) == true) {
    if (mupnp_httpu_socket_recv(server->httpuSock, ssdpPkt) <= 0)
      break;

    mupnp_ssdp_packet_print(ssdpPkt);

    mupnp_ssdpresponse_server_performlistener(server, ssdpPkt);
    mupnp_ssdp_packet_clear(ssdpPkt);
```

#### Reachability

Impact is limited to the targeted SSDP listener; host network reachability is required.

#### Severity

**Low** — Impact is limited to the targeted SSDP listener; host network reachability is required.

Additional runtime or deployment evidence could raise or lower this severity.

Impact assessment:
- **Level:** low
- **Why:** Impact is limited to the targeted SSDP listener; host network reachability is required.

Likelihood assessment:
- **Level:** high
- **Why:** Impact is limited to the targeted SSDP listener; host network reachability is required.

#### Remediation

Discard empty datagrams and distinguish them from socket errors; keep listener running.

Tests:
- Discard empty datagrams and distinguish them from socket errors; keep listener running. Verify malformed input fails cleanly and valid traffic still works.

Preventive controls:
- Make the repaired invariant mandatory in the shared helper.

## Reviewed Surfaces

| Surface | Risk Area | Outcome | Notes |
| --- | --- | --- | --- |
| Native HTTP, SSDP, POSIX lifecycle and TLS | not recorded | Reported | Seven baseline candidates independently validated. Severity calibrated to process versus subsystem impact and optional backend/lifecycle prerequisites. |
| Legacy media filesystem and XML ownership checks | not recorded | Rejected | Filesystem URI suffix selects an existing MD5 content ID; fopen uses stored scanner-owned path (cdms_filesys_http.c:80-96,130-133). RSS enclosure URL is hashed to objectID before shell filename arguments (crss.c:158-161; cdms_youtube_update.c:298-320), rejecting traced shell-injection lead. AV CGXmlNode copies parsed nodes before parser tree deletion (CGXmlNode.m:37-43,101-108). Operator stdin overflow in upnpdump.c:333-356 crosses no established privilege boundary. |
| Expat and libxml2 XML parsing, SOAP/control and URI utilities | not recorded | Reported | Two parent-validated XML findings. URI percent-escape OOB lead rejected: network strings are NUL terminated and short-circuit evaluation prevents the suspected trailing-character read. Libxml2 predefined-entity callback is present; external-entity disclosure was not established. One investigator final response was access-filtered; earlier observations and returned source coverage were retained. |
| Objective-C wrappers and cache/callback lifetimes | not recorded | Reported | Two independently validated lifetime findings. Retaining an Objective-C wrapper does not preserve its borrowed native object; raw callback userdata also needs a separate registration lifetime. |
| Media traversal examples and network test-device handler | not recorded | Reported | Container traversal and test-only stack disclosure calibrated to conditional utility/test exposure. Sibling traversal operations retained in source evidence. |
| Public API macros, limits and static service descriptors | not recorded | No issue found | Remaining headers reviewed. No new demonstrated boundary violation. Thread header promises termination before deletion; fixed-delay implementation contradicts it and is already reported. Fixed XML service descriptors contain no attacker-selected format/evaluation. Body, header, client and subscriber limits are absent; do not infer that buffer-size constants supply aggregate limits. |
| Build generators and CI publication routes | not recorded | No issue found | Fixed bootstrap/generator shell and Perl commands require trusted repository/operator authority. CI build executes PR code but master-only documentation publication is distinct. CMake emits CG_\* flags while consumers use MUPNP_\*; preserved as configuration discrepancy, not evidence of activated TLS or XML guards. |

## Open Questions And Follow Up

- Historical std/av YouTube target uses inconsistent legacy Cg_\* symbols/include paths. Raw URI suffix reaches unbounded strcpy into a small stack buffer at std/av/sample/upnpavserver/youtube/cdms_youtube_http.c:45,77-79, but working compilation/deployment was not established. Root builds omit std/av; preserve as deferred proof gap.
- CGUpnpDevice.m:388 uses uninitialized upnpAction in static delegate callback, but registration precedes service creation and later supported registration was not found.
- Actual deployment, OS permissions, external dependencies and effective flags were not supplied.
- Executable bodies but not every fixed literal were reviewed in: examples/binarylight/binarylight_device.c, examples/clock/clock_device.c, test/TestDevice.c, std/av/src/cybergarage/upnp/std/av/renderer/cavtransport_service.c, std/av/src/cybergarage/upnp/std/av/renderer/cconnectionmgrr_service.c, std/av/src/cybergarage/upnp/std/av/renderer/cmediarenderer_device.c, std/av/src/cybergarage/upnp/std/av/renderer/crenderingcontrol_service.c, std/av/src/cybergarage/upnp/std/av/server/cconnectionmgr_service.c, std/av/src/cybergarage/upnp/std/av/server/ccontentdir_service.c, std/av/src/cybergarage/upnp/std/av/server/cmediaserver_device.c. Ancillary historical project configurations and non-implementation artifacts remain outside full source coverage.
- Working compilation and active deployment of legacy target unresolved; not a validated current-library finding.
  - Follow-up prompt: Review deferred unit legacy-youtube-http-buffer and close its stated proof gap.
