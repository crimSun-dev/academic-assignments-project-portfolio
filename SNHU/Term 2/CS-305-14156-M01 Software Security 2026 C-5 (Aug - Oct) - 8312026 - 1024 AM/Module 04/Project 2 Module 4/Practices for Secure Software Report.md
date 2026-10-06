# Practices for Secure Software Report

**Client:** Artemis Financial
**Developer:** Draven Chen
**Company:** Global Rain
**Course:** CS 305, Software Security, Project Two
**Date:** September 14, 2026

## Document Revision History

| Version | Date | Author | Comments |
|---|---|---|---|
| 1.0 | 09/14/2026 | Draven Chen | Initial release: checksum endpoint, TLS configuration, dependency remediation. |

---

## 1. Algorithm Cipher

### Recommendation

For the file verification step, I recommend SHA-256, delivered over a TLS 1.3 or TLS 1.2 channel that encrypts traffic with AES-256-GCM and negotiates keys through ECDHE.

Those two components answer different questions, and conflating them would weaken the design. SHA-256 is a cryptographic hash function rather than a cipher; it encrypts nothing and takes no key. What it answers is whether a sequence of bytes has changed. AES-GCM, the actual cipher, answers whether anyone on the network can read or alter those bytes in transit. Artemis Financial asked for a checksum, which is an integrity control. Sent over plaintext HTTP, though, a checksum protects nothing, because an attacker positioned to modify the file can rewrite the published checksum in the same request. Only paired with an encrypted, authenticated channel does the checksum carry weight, so the refactored application deploys both.

### Overview of the algorithm

NIST published SHA-256 as part of the SHA-2 family in FIPS 180-4. Given input of any length, it returns a fixed 256-bit digest, written as 64 hexadecimal characters. Internally, the algorithm pads the message to a multiple of 512 bits and feeds each block through 64 rounds of modular addition, bitwise rotation, and XOR, chaining each block's output into the next (the Merkle–Damgård construction).

Three properties make it suitable for Artemis Financial:

- **Preimage resistance.** Nobody can work backward from a digest to the file that produced it.
- **Second-preimage resistance.** Given one file, nobody can build a different file with the same digest.
- **Collision resistance.** Nobody can find any two distinct inputs that share a digest.

File verification depends on the second and third. Should an attacker tamper with a client's retirement statement during transfer, the digest the client recomputes will no longer match the one Artemis Financial published. SHA-256 also exhibits a strong avalanche effect: flipping one input bit changes roughly half of the output bits. The refactored code demonstrates this concretely, and a unit test enforces it. Hashing `transfer $100` and `transfer $900` yields two digests with no visible relationship.

### Hash functions and bit levels

Digest length, measured in bits, sets the ceiling on security.

| Algorithm | Digest | Collision resistance | Status |
|---|---|---|---|
| MD5 | 128-bit | Broken | Collisions compute in seconds on a laptop. |
| SHA-1 | 160-bit | Broken (about 63-bit) | Deprecated after SHAttered (2017) and a chosen-prefix collision (2020). |
| **SHA-256** | **256-bit** | **128-bit** | **Recommended; no practical attack known.** |
| SHA-512 | 512-bit | 256-bit | Stronger and faster on 64-bit CPUs, at twice the digest size. |
| SHA3-256 | 256-bit | 128-bit | Built on a different internal design (the sponge construction). |

Why does a 256-bit digest yield only 128 bits of collision resistance? The birthday paradox explains it. A brute-force search for any colliding pair needs about 2^(n/2) attempts rather than 2^n, so SHA-256 demands roughly 2^128, or 3.4 × 10^38, operations. No adversary, state-sponsored or otherwise, can perform that many.

I chose SHA-256 over SHA-512 on practical grounds. Both resist every known attack. SHA-256 produces a digest half as long, a real saving when a checksum accompanies every file, appears in logs, and gets compared by eye. Hardware support also favors it, since Intel's SHA extensions and the ARMv8 cryptography extensions both accelerate SHA-256 directly. Artemis Financial gains nothing operational from 256 bits of collision resistance when 128 bits already lies beyond any attacker's reach.

### Random numbers, symmetric and asymmetric keys

Hash functions use neither keys nor randomness. Verification requires exactly that determinism: the same input must always produce the same digest, or the client could never recompute and compare. The same property, combined with SHA-256's speed, makes raw SHA-256 the wrong tool for storing passwords. An attacker holding a password database can test billions of guesses per second against unsalted SHA-256 hashes on commodity GPUs, which is why password storage calls for a deliberately slow, salted function such as bcrypt, scrypt, or Argon2id.

Keys and randomness enter at the transport layer, where the application relies on all three categories of cryptography at once.

- **Asymmetric cryptography.** The certificate carries an RSA-2048 key pair. Its private half never leaves the server, while every client receives the public half inside the certificate. RSA operations run slowly, so TLS uses them only to authenticate the server and help establish a shared secret, never to encrypt bulk traffic.
- **Symmetric cryptography.** AES-256-GCM encrypts the HTTP traffic itself. Both endpoints hold the same session key, and AES runs several orders of magnitude faster than RSA. GCM qualifies as an authenticated mode, providing confidentiality and integrity together, so flipped ciphertext bits cause decryption to fail instead of silently corrupting data.
- **Random numbers.** Without good randomness, TLS collapses. Each side contributes a random nonce to the handshake, ECDHE generates a fresh ephemeral key pair for every connection, and GCM needs a unique nonce for every record. A cryptographically secure pseudorandom generator, seeded from operating system entropy, supplies all of them. History shows what happens when that source fails. In 2008, a patch to Debian's OpenSSL package (CVE-2008-0166) shrank the effective seed space to 32,768 values, making every key generated on affected systems guessable. Two years later, researchers at fail0verflow recovered Sony's PlayStation 3 signing key because Sony reused the same ECDSA nonce for every signature. In both cases the mathematics held; the randomness did not.

ECDHE also buys forward secrecy. Because each session uses a throwaway key pair, an attacker who steals Artemis Financial's private key next year still cannot decrypt traffic recorded today. The cipher list in `application.properties` therefore contains only ECDHE-based suites and omits the older RSA key-transport suites, which offer no such protection.

### History and current state

Hash functions have followed a recurring cycle of adoption, erosion, and replacement.

- **MD5 (1992).** Ron Rivest published MD5 in RFC 1321, and it spread everywhere. Hans Dobbertin found weaknesses in its compression function in 1996; Xiaoyun Wang and her colleagues produced full collisions in 2004. By December 2008, a team including Alexander Sotirov and Marc Stevens had used an MD5 collision to forge a rogue certificate authority, and in 2012 the Flame malware exploited the same weakness to impersonate a Microsoft code-signing certificate.
- **SHA-1 (1995).** Designed as the successor, SHA-1 lingered for more than a decade after Wang's team published theoretical attacks in 2005. Google and CWI Amsterdam announced SHAttered, the first real SHA-1 collision, in February 2017. Gaëtan Leurent and Thomas Peyrin followed in 2020 with a chosen-prefix collision costing roughly $45,000 in rented GPU time, a far more dangerous result because it lets an attacker collide two meaningful documents of their choosing.
- **SHA-2 (2001).** Twenty-five years on, SHA-256 has resisted every published attack. TLS certificate signatures, Bitcoin's proof-of-work, and most modern code-signing systems depend on it, and Git added SHA-256 repositories as the planned successor to its SHA-1 object IDs.
- **SHA-3 (2015).** NIST standardized Keccak as SHA-3 in FIPS 202, not because SHA-2 had failed but as insurance. Its sponge construction shares nothing structurally with the Merkle–Damgård design, so an attack on one would not automatically transfer to the other.

A pattern emerges from that timeline. Between the first published weakness and a practical attack, roughly a decade passed for both MD5 and SHA-1. Organizations treated that decade as slack and then migrated under pressure anyway. Artemis Financial should revisit its algorithm choices on a schedule rather than treating them as settled.

Quantum computing defines the next horizon. Shor's algorithm, run on a sufficiently large quantum computer, would break RSA and elliptic-curve key exchange outright; NIST responded in August 2024 by publishing its first post-quantum standards, ML-KEM (FIPS 203) and ML-DSA (FIPS 204). Hash functions fare much better. Grover's algorithm only halves their effective strength, leaving SHA-256 with 128 bits of preimage resistance, so migration priorities fall on key exchange and signatures first. Placing every hash operation inside one `ChecksumService` class hedges against the eventual change: swapping algorithms means editing one constant in one file.

---

## 2. Certificate Generation

Using the Java Keytool, I generated a self-signed X.509 certificate:

```
keytool -genkeypair \
  -alias artemis \
  -keyalg RSA -keysize 2048 -sigalg SHA256withRSA \
  -storetype PKCS12 \
  -keystore src/main/resources/artemis-keystore.p12 \
  -storepass <password> \
  -validity 365 \
  -dname "CN=localhost, OU=Global Rain, O=Artemis Financial, L=Manchester, ST=NH, C=US" \
  -ext "SAN=dns:localhost,ip:127.0.0.1" \
  -ext "KeyUsage=digitalSignature,keyEncipherment" \
  -ext "ExtendedKeyUsage=serverAuth"
```

Then I exported it as a CER file:

```
keytool -exportcert -alias artemis \
  -keystore src/main/resources/artemis-keystore.p12 \
  -storepass <password> -rfc -file artemis-cert.cer
```

Running `keytool -printcert -file artemis-cert.cer` confirmed the result:

```
Owner:     CN=localhost, OU=Global Rain, O=Artemis Financial, L=Manchester, ST=NH, C=US
Issuer:    CN=localhost, OU=Global Rain, O=Artemis Financial, L=Manchester, ST=NH, C=US
Serial number: 198ef09877c84ae
Valid from: Mon Sep 14 13:57:58 2026 until: Tue Sep 14 13:57:58 2027
SHA256 fingerprint: 52:4A:01:C9:D7:FE:B4:35:B9:51:BA:36:C4:84:A2:19:
                    EE:C1:A4:B4:1A:93:B1:02:F5:52:06:CD:D0:C5:66:54
Signature algorithm: SHA256withRSA
Subject Public Key Algorithm: 2048-bit RSA key
Version: 3
```

Keytool's defaults would have produced a weaker certificate, so each option above reflects a choice.

- **PKCS#12 rather than JKS.** JKS is a proprietary Java format. JDK 9 replaced it as the default keystore type (JEP 229) with PKCS#12, an open standard that other platforms read without conversion.
- **SHA256withRSA signature.** Section 1's algorithm reappears here: the certificate's own signature consists of a SHA-256 digest signed with the RSA private key. Older tooling defaulted to SHA-1 at this step, the very weakness exploited to forge certificate authorities.
- **RSA at 2048 bits.** NIST SP 800-57 accepts 2048-bit RSA keys as providing 112 bits of security through 2030.
- **A Subject Alternative Name.** Since Chrome 58 in 2017, browsers have ignored the Common Name field when matching hostnames. A certificate carrying only `CN=localhost` fails validation in every modern browser, a mistake that trips up many first attempts at this assignment.
- **Restricted key usage.** The KeyUsage and ExtendedKeyUsage extensions authorize TLS server authentication and nothing else, applying least privilege to a credential.

No browser trusts a self-signed certificate, and Chrome will display a warning before loading the page. That warning confirms certificate validation works as designed. Self-signed certificates suit development only. In production, Artemis Financial needs a certificate issued by a trusted authority such as Let's Encrypt or DigiCert, because a self-signed certificate encrypts traffic without authenticating the server. An attacker could present a self-signed certificate of their own, and a client with no trusted issuer to check against would have no way to tell the difference.

> **[INSERT SCREENSHOT 1: The exported artemis-cert.cer file]**
> Exported certificate inspected with keytool -printcert.

---

## 3. Deploy Cipher

The skeleton contained no checksum code, only a comment:

```java
//FIXME: Add route to enable check sum return of static data example: String data = "Hello World Check Sum!";
```

I split the implementation across two classes, separating the cryptography from the HTTP handling.

**`ChecksumService.java`** holds every hashing operation in the application. It declares the algorithm once, as a constant, and offers callers no way to request a weaker one. Consolidation matters in practice. When each feature carries its own copy of hashing code, one endpoint can move to SHA-256 while another quietly keeps MD5 for years.

```java
public static final String ALGORITHM = "SHA-256";

public String hexDigest(String data) {
    if (data == null) {
        throw new IllegalArgumentException("data must not be null");
    }
    try {
        MessageDigest digest = MessageDigest.getInstance(ALGORITHM);
        byte[] hash = digest.digest(data.getBytes(StandardCharsets.UTF_8));
        return HexFormat.of().formatHex(hash);
    } catch (NoSuchAlgorithmException e) {
        throw new IllegalStateException(ALGORITHM + " is not available in this JVM", e);
    }
}
```

Three details in that code guard against mistakes that commonly slip through review:

1. **UTF-8 is pinned explicitly.** Called without an argument, `data.getBytes()` uses the platform's default character set, so identical text can hash differently on a Windows workstation and a Linux server. Nothing would crash; the checksums would simply disagree, and tracing the cause could take days.
2. **The service fails closed.** If a JVM ever lacked SHA-256, the method throws an exception instead of falling back to a weaker algorithm.
3. **Comparison runs in constant time.** The `verify()` method calls `MessageDigest.isEqual()` rather than `String.equals()`. Because `String.equals()` stops at the first mismatched character, its running time reveals how many leading characters of a guess were correct, and an attacker measuring response times could recover a digest one character at a time.

**`ChecksumController.java`** exposes the service at `GET /hash`, returning the developer's name, the data string, the algorithm, and the digest. Loading `https://localhost:8443/hash` produced this output:

```
Name:            Draven Chen
Data String:     Artemis Financial secure transfer verification string
Algorithm:       SHA-256
Checksum (hex):  f5d7d071b61b63022962a1e6298bf1f3899a401932d04cd2a42c6a05472fefb2
```

To confirm the digest independently, I hashed the same string with the operating system's `sha256sum` utility and got an identical result:

```
$ printf '%s' "Artemis Financial secure transfer verification string" | sha256sum
f5d7d071b61b63022962a1e6298bf1f3899a401932d04cd2a42c6a05472fefb2
```

The endpoint also accepts a custom `data` parameter. For the verification screenshot I supplied a unique data string, and once again the browser's digest matched `sha256sum` character for character:

```
https://localhost:8443/hash?data=Draven Chen - CS-305 Artemis Financial unique verification 2026-09-14

Checksum (browser):  60c94ed33203fab0dd5650210b4082878e75f77059e57683e5e6d33749919a67
$ printf '%s' "Draven Chen - CS-305 Artemis Financial unique verification 2026-09-14" | sha256sum
                     60c94ed33203fab0dd5650210b4082878e75f77059e57683e5e6d33749919a67
```

Unit tests add a third check, comparing the implementation against NIST's published test vectors for `"abc"` and the empty string. Agreement with both an external tool and the official vectors establishes that the code computes genuine SHA-256, not merely output shaped like a hash.

> **[INSERT SCREENSHOT 2: Checksum verification]**
> Checksum page showing name, unique data string, and checksum.

---

## 4. Secure Communications

The skeleton's `application.properties` could not start a server. Every TLS setting held a placeholder:

```properties
server.port=8443
server.ssl.key-alias=????
server.ssl.key-store-password=????
server.ssl.key-store=????
server.ssl.key-store-type=????
```

Filling in those four values would have enabled HTTPS. The refactored configuration goes further:

```properties
server.port=8443
server.ssl.enabled=true
server.ssl.key-alias=artemis
server.ssl.key-store=classpath:artemis-keystore.p12
server.ssl.key-store-type=PKCS12
server.ssl.key-store-password=${ARTEMIS_KEYSTORE_PASSWORD:artemis2026}

# RFC 8996 deprecates TLS 1.0 and 1.1 (POODLE, BEAST)
server.ssl.enabled-protocols=TLSv1.3,TLSv1.2
# Forward-secret AEAD suites only: no RSA key transport, no CBC, no RC4
server.ssl.ciphers=TLS_AES_256_GCM_SHA384,TLS_AES_128_GCM_SHA256,\
TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256

# Do not leak stack traces or the container version to attackers
server.error.include-stacktrace=never
server.error.include-message=never
server.server-header=
```

Plain HTTP gets no listener at all, not even a redirect to HTTPS. A redirect still begins with one unencrypted request, and an attacker on the network path can intercept that request before the redirect ever arrives. Port 8443 accepts TLS connections and nothing else.

### Verification

Opening `https://localhost:8443/hash` in a browser returns the checksum page over an encrypted connection. Command-line tools confirmed the details:

```
$ openssl s_client -connect localhost:8443 -CAfile artemis-cert.cer -brief
Protocol version: TLSv1.3
Ciphersuite: TLS_AES_256_GCM_SHA384
Verification: OK
```

```
$ curl --cacert artemis-cert.cer https://localhost:8443/hash -w "%{ssl_verify_result}"
... ssl_verify_result=0   (0 = certificate validated successfully)
```

"Verification: OK" carries real weight. OpenSSL checked the certificate the live server presented against the CER file exported in Section 2 and found them cryptographically identical, tying the two deliverables together.

Configuring protocol restrictions and enforcing them are separate matters, so I tested enforcement directly. A client forced to speak TLS 1.0 was refused during the handshake:

```
$ curl --tlsv1.0 --tls-max 1.0 https://localhost:8443/hash
curl: (35) schannel: next InitializeSecurityContext failed:
      SEC_E_UNSUPPORTED_FUNCTION - The function requested is not supported
```

### HTTP hardening

TLS protects data in transit but tells the browser nothing about handling the response once it arrives. `SecurityConfig.java` fills that gap. The live server returned these headers:

```
Strict-Transport-Security: max-age=31536000 ; includeSubDomains
Content-Security-Policy: default-src 'none'; style-src 'self'; frame-ancestors 'none'
X-Content-Type-Options: nosniff
X-Frame-Options: DENY
Referrer-Policy: no-referrer
Cache-Control: no-cache, no-store, max-age=0, must-revalidate
```

Each header closes a specific opening. Strict-Transport-Security instructs the browser to refuse plaintext connections to this host for one year, shutting the window an SSL-stripping attacker would otherwise exploit on repeat visits. Content-Security-Policy declares that the page may load scripts from no origin whatsoever; even if output encoding someday failed, an injected `<script>` tag would have nowhere permitted to run from. With `nosniff` set, the browser stops guessing at content types and will not execute a text response as script. `X-Frame-Options: DENY` blocks clickjacking by forbidding the page from appearing inside a frame. `Referrer-Policy: no-referrer` keeps the query string, which may contain the very data under verification, out of requests sent to third-party sites. Finally, the response omits the `Server` header, denying anyone fingerprinting the application a free report of its container and version.

> **[INSERT SCREENSHOT 3: Secure web page]**
> Browser at https://localhost:8443/hash.

---

## 5. Secondary Testing

### Repairing the scanner first

Before scanning anything, I had to fix the scanner. The skeleton's `pom.xml` pinned `dependency-check-maven` at version 5.3.0, which reads NIST's legacy NVD 1.0 data feeds. NIST retired those feeds in December 2023. Version 5.3.0 therefore either fails outright or, worse, scans against a years-old local database and reports a clean result. A scanner that reports clean only because it lacks current data produces false confidence, which does more harm than having no scanner at all. I upgraded the plugin to version 12.1.0, which queries the NVD 2.0 API.

One analyzer then broke the build. Sonatype's OSS Index now rejects unauthenticated requests with HTTP 401, so I disabled that single analyzer and documented the reason in the POM. NVD data remained fully active. In a CI pipeline with account credentials, OSS Index should return as a second source.

### Four iterations to a clean result

The assignment calls for iterating until no new vulnerabilities remain. Reaching that point took four passes.

**Iteration 1: Spring Boot 2.2.4.RELEASE.** Released in January 2020, this version still compiles and starts on Java 21; I confirmed it launches and answers HTTP requests. Running is not the problem. The Spring Boot 2.x line reached end of open-source support in November 2023, so nothing released for it since then patches the Spring Framework, Tomcat, Jackson, or SnakeYAML versions it pulls in. I did not scan this version, because its bundled 5.3.0 scanner could no longer download vulnerability data, and I moved to a supported release before rerunning the scan.

**Iteration 2: Spring Boot 3.5.6.** The scan reported **143 vulnerabilities** across 11 artifacts, with Critical findings in `spring-core`, `spring-security-core`, and `tomcat-embed-core`. Version 3.5.6 had itself fallen about a year behind the NVD data. Upgrading once, in other words, does not finish the job; a version that looked current when a project began can carry dozens of CVEs by the following semester.

**Iteration 3: Spring Boot 4.1.1, the current release.** Findings dropped to **11 vulnerabilities**, all inside a single artifact, `tomcat-embed-core-11.0.24.jar`: four Critical (CVE-2026-65637, CVE-2026-65905, CVE-2026-65182, CVE-2026-68525), five High, and two Medium.

**Iteration 4: pinning the transitive dependency.** Spring Boot 4.1.1 manages Tomcat at 11.0.24, but Tomcat 11.0.25 had already fixed those CVEs. Since Tomcat arrives transitively, upgrading Spring Boot alone could not reach it, and I overrode the managed version in the POM:

```xml
<tomcat.version>11.0.25</tomcat.version>
```

The final scan came back clean:

```
dependency-check version:       12.1.0
NVD data source last modified:  2026-09-02
Artifacts scanned:              33
Total vulnerabilities:           0
```

From 143 to 11 to 0. After every change, I reran all eleven tests, and they passed each time. A dependency upgrade that silently breaks application behavior trades one defect for another.

One caveat belongs on the record. The local NVD database used for these scans was last refreshed on September 2, 2026, twelve days before the final scan, so any vulnerability published in that interval would not appear. Artemis Financial's pipeline should refresh NVD data on every run using a registered API key and rescan on a fixed schedule, since a dependency can become vulnerable without anyone changing a line of code.

> **[INSERT SCREENSHOT 4: Dependency-check report]**
> Dependency-check report summary showing 0 vulnerabilities.
>
> **[INSERT SCREENSHOT 5: Refactored code executing without errors]**
> mvn clean test completing with BUILD SUCCESS.

---

## 6. Functional Testing

Automated tools catch known CVEs and common injection patterns. Logic flaws, authorization gaps, and problems specific to one business escape them. I therefore reviewed every source file by hand, sorting defects into three categories.

### Syntactical

| # | Finding | Resolution |
|---|---|---|
| S1 | `application.properties` held literal `????` placeholders, so the application could not start. | Replaced with a working, hardened TLS configuration. |
| S2 | The required checksum route did not exist; only a `FIXME` comment marked its place. | Implemented `ChecksumController` and `ChecksumService`. |
| S3 | `java.version` targeted Java 1.8, a 2014 release that current Spring Boot versions no longer support (Spring Boot 3 and later require Java 17). | Moved to Java 21 LTS. |

### Logical

| # | Finding | Resolution |
|---|---|---|
| L1 | `String.getBytes()` without an explicit charset yields different digests on different platforms. | Pinned to `StandardCharsets.UTF_8`. |
| L2 | A null `data` value would raise a `NullPointerException` and return a 500 error with a stack trace. | Added an explicit null check; blank input falls back to a default payload. |
| L3 | Nothing verified that the code produced correct SHA-256 output. | Added tests against NIST vectors and cross-checked with `sha256sum`. |
| L4 | Nothing verified that tampering changes the checksum. | Added a test asserting that `transfer $100` and `transfer $900` hash differently. |

### Security

| # | Finding | Severity | Resolution |
|---|---|---|---|
| V1 | The application could not serve HTTPS, leaving all traffic unencrypted. | **Critical** | Enabled TLS 1.3 and 1.2 only, with forward-secret AEAD suites and no plaintext listener. |
| V2 | The `data` parameter is echoed into HTML, so `GET /hash?data=<script>...` would execute script in a victim's browser (reflected XSS). | **High** | Encoded all output with `HtmlUtils.htmlEscape()`; a regression test guards the fix. |
| V3 | Outdated dependencies carried 143 known CVEs. | **Critical** | Upgraded and pinned versions until the scan reported zero (Section 5). |
| V4 | Comparing digests with `String.equals()` leaks, through timing, how much of a guess was correct. | **Medium** | Switched to constant-time `MessageDigest.isEqual()`. |
| V5 | An arbitrarily long query string could tie up CPU and memory. | **Medium** | Capped reflected input at 1,024 characters and limited header and form sizes. |
| V6 | Error responses exposed stack traces and the container version. | **Medium** | Set `include-stacktrace=never` and `include-message=never`, and removed the `Server` header. |
| V7 | Any endpoint added later would be publicly reachable by default. | **Medium** | Adopted a fail-closed policy: `/hash` is explicitly public, and every other request requires authentication. |
| V8 | Responses carried no browser hardening headers. | **Low** | Added HSTS, CSP, `nosniff`, `X-Frame-Options: DENY`, and `Referrer-Policy`. |

### Regression coverage

Wherever a fix could be expressed as a test, I wrote one, so no later change can quietly undo it. All eleven tests pass:

```
ChecksumServiceTests       6 tests, 0 failures, 0 errors
ChecksumControllerTests    4 tests, 0 failures, 0 errors
SslServerApplicationTests  1 test,  0 failures, 0 errors
```

`ChecksumControllerTests` confirms that `/hash?data=<script>alert(1)</script>` returns escaped text (V2), that hardening headers accompany every response (V8), and that undeclared routes are refused (V7). Against the live server, the XSS payload came back inert:

```
$ curl --cacert artemis-cert.cer -G --data-urlencode "data=<script>alert(1)</script>" \
       https://localhost:8443/hash
Data String:</strong> &lt;script&gt;alert(1)&lt;/script&gt;
```

The browser displays that payload as ordinary text and never runs it.

### Known limitations

Three weaknesses remain, and omitting them would misrepresent the application's security.

- **The keystore password sits in `application.properties`.** It reads from the `ARTEMIS_KEYSTORE_PASSWORD` environment variable but falls back to a default, acceptable for a self-signed demonstration and unacceptable in production. Artemis Financial should move the secret into a manager such as AWS KMS or HashiCorp Vault.
- **The certificate is self-signed.** It encrypts traffic but cannot prove the server's identity; production requires a certificate from a trusted authority.
- **The `/hash` endpoint requires no authentication.** The assignment calls for demonstrating it in a browser. A production deployment handling client financial data would place authentication and per-user rate limiting in front of it.

---

## 7. Summary

### Areas of the vulnerability assessment process addressed

The Vulnerability Assessment Process Flow Diagram begins with an architecture review, moves through seven security areas, and then directs manual code reviews according to what the architecture contains. I followed that sequence.

| Diagram area | How the refactor addressed it |
|---|---|
| **Architecture Review** | Mapped the application as a single Spring Boot service: one REST controller, one service, one security configuration, and no database. That map determined which code reviews applied. |
| **Input Validation** | Rejects null input, caps reflected data at 1,024 characters, limits header and form sizes, and pins UTF-8 so input bytes stay consistent across platforms. |
| **APIs** | Uses the JDK's `MessageDigest` and Spring's `HtmlUtils` exactly as documented, and exposes `GET /hash` as the only public route. |
| **Cryptography** | SHA-256 checksums with constant-time comparison; TLS 1.3 and 1.2 only, with AES-256-GCM and ECDHE forward secrecy; RSA-2048 certificate signed with SHA-256. |
| **Client/Server** | HTTPS with no plaintext listener, HSTS, CSP, frame blocking, and a strict referrer policy. |
| **Code Error** | Fails closed if SHA-256 is unavailable; suppresses stack traces, exception messages, and the `Server` header in error responses. |
| **Code Quality** | Separates hashing from HTTP handling, uses constructor injection, and backs every fix with one of eleven passing tests. |
| **Encapsulation** | Keeps the algorithm in a `public static final` constant and the service dependency in a `private final` field, so no caller can swap in a weaker algorithm. |

The architecture review ruled several code reviews in and a few out:

| Code review | Applied | Scope |
|---|---|---|
| **Controllers** | Yes | `ChecksumController`: found and fixed reflected XSS (V2) and unbounded input (V5). |
| **Services** | Yes | `ChecksumService`: fixed charset handling (L1), null handling (L2), and timing leaks (V4). |
| **Views** | Yes | The HTML response the controller builds: confirmed output encoding and hardening headers. |
| **APIs** | Yes | Correct use of `MessageDigest`, `HtmlUtils`, and Spring Security's configuration API. |
| **Plug-Ins** | Yes | Maven plugins and third-party libraries: replaced the defunct dependency-check 5.3.0 and cut known CVEs from 143 to 0. |
| **Models** | No | The application defines no domain model classes. |
| **Data Access** | No | The application has no database or persistence layer. |

The diagram ends with a summary of findings and a mitigation plan; Section 6 supplies both, listing fifteen findings with a severity and resolution for each.

### Adding layers of security

I built outward from the data, completing and verifying each layer before starting the next.

1. **Data integrity:** SHA-256 checksums in one service, with constant-time verification.
2. **Transport security:** TLS 1.3 and 1.2 with forward-secret suites, a certificate carrying a proper SAN, and no plaintext listener.
3. **Input validation:** null rejection, length caps, and request limits applied where untrusted data enters.
4. **Output encoding:** HTML escaping on everything reflected back to the client.
5. **Access control:** a fail-closed policy, so a route added six months from now stays unreachable until someone deliberately opens it.
6. **Browser hardening:** HSTS, CSP, frame blocking, and a strict referrer policy.
7. **Supply chain:** current dependencies, pinned transitive versions, and scanning built into the Maven lifecycle.
8. **Regression testing:** a test for each fix, so no layer can disappear unnoticed.

No single layer deserves full trust, and the arrangement reflects that. Output encoding should stop XSS, yet if a future developer forgets to encode a new field, CSP still blocks the injected script. TLS should prevent interception, yet if a user's very first request travels over plaintext, HSTS protects every visit after it. Each layer sits where it can catch what the previous one missed.

Two decisions about process shaped the result. I refactored the dependencies before writing any checksum code, because features built on a framework carrying 143 known CVEs would need rework once the framework changed. And I verified every fix rather than assuming it. The checksum matched both the NIST vectors and an external tool; the TLS configuration negotiated TLS 1.3 and refused TLS 1.0 under test; the XSS fix neutralized a live payload. A fix that nobody tests invites confidence it has not earned.

---

## 8. Industry Standard Best Practices

### Standards applied

- **OWASP Top 10 (2021):** A01 Broken Access Control (fail-closed authorization), A02 Cryptographic Failures (SHA-256, TLS 1.3, forward secrecy), A03 Injection (output encoding), A05 Security Misconfiguration (error suppression, hardening headers), and A06 Vulnerable and Outdated Components (dependency remediation).
- **OWASP ASVS:** V6 Stored Cryptography (approved algorithms, no custom primitives), V9 Communications (TLS 1.2 or newer, strong suites), V5 Validation and Encoding, and V14 Configuration.
- **NIST SP 800-131A Rev. 2:** SHA-256 and 2048-bit RSA both meet its minimum strength requirements.
- **NIST SSDF (SP 800-218):** practices PW.4 (reuse well-secured software), PW.7 (review code), PW.8 (test executable code), and RV.1 (identify vulnerabilities continuously).
- **RFC 8996:** formal deprecation of TLS 1.0 and 1.1, enforced in the configuration.
- **FIPS 180-4:** the SHA-256 specification itself.

### Preserving existing security

Refactoring can strip out protections as easily as it adds them. Three habits prevented that here.

First, no protection was removed simply to make something else easier. Spring Security's CSRF protection is disabled, but only because `/hash` is a stateless, read-only GET request with no session for an attacker to ride, and a comment at that line records the reasoning. A reviewer who later finds `.csrf(csrf -> csrf.disable())` will see why it exists instead of guessing whether someone made a mistake.

Second, the test suite ran after every change, including all four dependency iterations. I declared nothing fixed because it looked correct.

Third, the code relies on established framework components wherever one exists: `MessageDigest` for hashing, `HtmlUtils.htmlEscape()` for encoding, and Spring Security's header writers for response headers. Custom cryptography and homemade HTML escapers have a long record of introducing subtle, high-severity bugs; an escaper that mishandles a single character in a single context can reopen an entire class of XSS attacks.

### Value to Artemis Financial

Artemis Financial holds its clients' savings, retirement, investment, and insurance records. Secure coding practices protect that business in four concrete ways.

**Cost.** IBM's 2024 Cost of a Data Breach Report put the average breach in the financial sector at $6.08 million, well above the $4.88 million average across all industries. In this project, eliminating 143 known vulnerabilities took an afternoon of dependency work. Finding the same exposure through an incident would instead mean paying for forensics, legal counsel, regulatory notification, and remediation for every affected client.

**Regulation.** A firm handling client financial data falls under the FTC's Safeguards Rule issued under the Gramm-Leach-Bliley Act, along with state breach-notification laws. Encryption in transit and documented vulnerability management serve as baseline expectations under that regime, not optional extras.

**Trust.** Clients give Artemis Financial a complete picture of their financial lives. A breach would damage that relationship far more than any single technical fix could repair, which makes security part of what Artemis Financial sells rather than overhead.

**Speed of change.** Security investment pays off when the code must evolve. With hashing centralized, a future move to a post-quantum or SHA-3 algorithm touches one file. With scanning in the build, a vulnerable dependency surfaces at commit time instead of during an audit months later. With regression tests in place, a fix stays fixed. Well-engineered security makes an application safer to change, not slower.

Global Rain's own motto holds that security is everyone's responsibility. That principle only survives deadline pressure when the secure path is also the easy one, because developers forced to choose between shipping quickly and shipping securely will usually ship quickly. Shared services, framework components, automated scanning, and regression tests all push toward the same outcome: making the secure choice the default one, so that protection does not depend on someone remembering to be careful.

---

## Attachments

| File | Description |
|---|---|
| `src/main/java/com/snhu/sslserver/ChecksumService.java` | SHA-256 hashing and constant-time verification |
| `src/main/java/com/snhu/sslserver/ChecksumController.java` | `GET /hash` endpoint with input validation and output encoding |
| `src/main/java/com/snhu/sslserver/SecurityConfig.java` | Access control and HTTP hardening headers |
| `src/main/java/com/snhu/sslserver/SslServerApplication.java` | Application entry point |
| `src/main/resources/application.properties` | TLS configuration |
| `src/main/resources/artemis-keystore.p12` | PKCS#12 keystore |
| `artemis-cert.cer` | Exported self-signed certificate |
| `pom.xml` | Dependencies and dependency-check plugin |
| `owasp-suppression.xml` | Suppression policy (empty; nothing suppressed) |
| `src/test/java/com/snhu/sslserver/*.java` | Eleven unit and security regression tests |
| `dependency-check-report.html` | Final scan: 0 vulnerabilities across 33 artifacts |
