package com.snhu.sslserver;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

/**
 * Verifies the cryptographic hashing primitive itself.
 *
 * <p>These are correctness tests against published NIST vectors. A checksum routine that
 * merely "returns something that looks like a hash" is worse than no checksum at all,
 * because it creates false confidence in the integrity of the transfer.
 */
class ChecksumServiceTests {

	private final ChecksumService service = new ChecksumService();

	@Test
	@DisplayName("matches the published SHA-256 test vector for \"abc\"")
	void knownVector() {
		assertEquals(
				"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
				service.hexDigest("abc"));
	}

	@Test
	@DisplayName("matches the published SHA-256 test vector for the empty string")
	void emptyVector() {
		assertEquals(
				"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
				service.hexDigest(""));
	}

	@Test
	@DisplayName("digest is 64 hex characters (256 bits)")
	void digestLength() {
		String digest = service.hexDigest("Artemis Financial");
		assertEquals(64, digest.length());
		assertTrue(digest.matches("[0-9a-f]{64}"), "digest must be lowercase hex");
	}

	@Test
	@DisplayName("a single changed character produces a completely different digest")
	void avalanche() {
		assertFalse(service.hexDigest("transfer $100").equals(service.hexDigest("transfer $900")),
				"tampered payload must not produce the same checksum");
	}

	@Test
	@DisplayName("null input is rejected rather than NPE-ing out of the service")
	void nullRejected() {
		assertThrows(IllegalArgumentException.class, () -> service.hexDigest(null));
	}

	@Test
	@DisplayName("verify() accepts the correct digest and rejects a wrong one")
	void verification() {
		String data = "Artemis Financial";
		assertTrue(service.verify(data, service.hexDigest(data)));
		assertFalse(service.verify(data, "0".repeat(64)));
		assertFalse(service.verify(data, null));
	}
}
