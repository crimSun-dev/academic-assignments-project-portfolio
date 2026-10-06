# Artemis Financial — Refactored Secure Application

CS 305 Project Two. Spring Boot service exposing a SHA-256 checksum endpoint over TLS.

## Requirements
- JDK 21 or later (built and verified on Zulu 21.0.12)
- Maven 3.9+

## Run
```
mvn clean spring-boot:run
```
Then open <https://localhost:8443/hash>

The certificate is self-signed, so the browser shows a warning.
Click **Advanced -> Proceed to localhost (unsafe)**. The connection is still
encrypted; the warning means certificate validation is working correctly.

## Test
```
mvn clean test
```
11 tests: SHA-256 correctness against NIST vectors, plus security regression
tests for XSS escaping, hardening headers, and fail-closed authorization.

## Dependency scan
```
mvn verify -Dnvd.api.key=YOUR_KEY
```
Get a free key at <https://nvd.nist.gov/developers/request-an-api-key>.
To scan against an already-downloaded NVD database without contacting NIST:
```
mvn org.owasp:dependency-check-maven:12.1.0:check -DautoUpdate=false -Dossindex.analyzer.enabled=false
```
Report: `target/dependency-check-report.html`

## Regenerate the certificate
```
keytool -genkeypair -alias artemis -keyalg RSA -keysize 2048 \
  -sigalg SHA256withRSA -storetype PKCS12 \
  -keystore src/main/resources/artemis-keystore.p12 \
  -storepass artemis2026 -validity 365 \
  -dname "CN=localhost, OU=Global Rain, O=Artemis Financial, L=Manchester, ST=NH, C=US" \
  -ext "SAN=dns:localhost,ip:127.0.0.1" \
  -ext "KeyUsage=digitalSignature,keyEncipherment" \
  -ext "ExtendedKeyUsage=serverAuth"

keytool -exportcert -alias artemis \
  -keystore src/main/resources/artemis-keystore.p12 \
  -storepass artemis2026 -rfc -file artemis-cert.cer
```

## Security notes
- The keystore password is inline only because this is a self-signed demo.
  Production must read it from a secrets manager, not from source control.
- The certificate is self-signed: encryption without authentication.
  Production requires a CA-issued certificate.
- `/hash` is intentionally unauthenticated so it can be demonstrated in a browser.
