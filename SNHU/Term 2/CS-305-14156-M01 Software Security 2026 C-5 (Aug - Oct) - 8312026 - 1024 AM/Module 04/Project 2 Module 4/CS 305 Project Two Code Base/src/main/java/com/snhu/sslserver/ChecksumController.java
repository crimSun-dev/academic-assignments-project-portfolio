package com.snhu.sslserver;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.util.HtmlUtils;

/**
 * Checksum verification endpoint for Artemis Financial.
 *
 * <p>Fulfils the skeleton's {@code FIXME: Add route to enable check sum return of static
 * data}. A client that downloads a file from Artemis Financial can call this route,
 * hash the bytes it received, and compare the two digests. If they differ the payload
 * was altered in transit or at rest.
 */
@RestController
public class ChecksumController {

	/** Hard cap on reflected input. Prevents an attacker from using the endpoint as a
	 *  CPU/memory amplifier by posting a multi-megabyte query string. */
	private static final int MAX_DATA_LENGTH = 1024;

	/** Default payload hashed when no {@code data} parameter is supplied. */
	private static final String DEFAULT_DATA =
			"Artemis Financial secure transfer verification string";

	private final ChecksumService checksumService;

	/** Externalised so the name is configuration, not a hardcoded literal in the source. */
	@Value("${artemis.developer-name:Draven Chen}")
	private String developerName;

	/** Constructor injection: the dependency is final, explicit, and trivially testable. */
	public ChecksumController(ChecksumService checksumService) {
		this.checksumService = checksumService;
	}

	/**
	 * Returns the SHA-256 checksum of a data string.
	 *
	 * @param data optional payload to hash; defaults to {@link #DEFAULT_DATA}
	 * @return an HTML fragment naming the developer, the data, the algorithm, and the digest
	 */
	@GetMapping(value = "/hash", produces = MediaType.TEXT_HTML_VALUE)
	public String hash(@RequestParam(name = "data", required = false) String data) {

		// --- Input validation at the trust boundary -------------------------------
		// Anything arriving on the query string is untrusted. Normalise it to a known
		// safe shape before it reaches the hashing routine or the response body.
		String payload = (data == null || data.isBlank()) ? DEFAULT_DATA : data.trim();
		if (payload.length() > MAX_DATA_LENGTH) {
			payload = payload.substring(0, MAX_DATA_LENGTH);
		}

		String digest = checksumService.hexDigest(payload);

		// --- Output encoding (A03:2021 - Injection / XSS) -------------------------
		// The payload is echoed back into an HTML response, so it is HTML-escaped on the
		// way out. Without this, GET /hash?data=<script>... would be a reflected XSS
		// against every Artemis Financial customer who could be sent the link.
		String safePayload = HtmlUtils.htmlEscape(payload);
		String safeName = HtmlUtils.htmlEscape(developerName);

		return "<!DOCTYPE html><html><head><title>Artemis Financial Checksum</title></head><body>"
				+ "<h2>Artemis Financial &mdash; File Verification</h2>"
				+ "<p><strong>Name:</strong> " + safeName + "</p>"
				+ "<p><strong>Data String:</strong> " + safePayload + "</p>"
				+ "<p><strong>Algorithm:</strong> " + ChecksumService.ALGORITHM + "</p>"
				+ "<p><strong>Checksum (hex):</strong> " + digest + "</p>"
				+ "</body></html>";
	}
}
