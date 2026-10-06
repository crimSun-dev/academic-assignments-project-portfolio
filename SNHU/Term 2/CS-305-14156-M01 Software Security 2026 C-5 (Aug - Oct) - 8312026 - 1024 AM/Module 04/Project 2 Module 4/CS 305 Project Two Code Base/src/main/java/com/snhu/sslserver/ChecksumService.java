package com.snhu.sslserver;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HexFormat;

import org.springframework.stereotype.Service;

/**
 * Central home for Artemis Financial's cryptographic hashing.
 *
 * <p>Every checksum in the application goes through this one class. Hashing logic that
 * is copy-pasted per feature is how an application ends up with one endpoint on SHA-256
 * and another still on MD5 two years later, so the algorithm is declared exactly once,
 * here, and callers cannot choose a weaker one.
 *
 * <p><b>Algorithm:</b> SHA-256 (FIPS 180-4). MD5 and SHA-1 are both broken for collision
 * resistance -- practical chosen-prefix collisions exist for both -- and must never be
 * used for the file-verification step this service backs.
 */
@Service
public class ChecksumService {

	/**
	 * The only hash algorithm this application uses. SHA-256 produces a 256-bit digest,
	 * giving 128 bits of collision resistance under the birthday bound, which is the
	 * current industry baseline (NIST SP 800-131A Rev. 2).
	 */
	public static final String ALGORITHM = "SHA-256";

	/**
	 * Computes the SHA-256 digest of {@code data} and returns it as lowercase hex.
	 *
	 * @param data the text to hash; must not be {@code null}
	 * @return the 64-character hexadecimal digest
	 * @throws IllegalArgumentException if {@code data} is {@code null}
	 * @throws IllegalStateException    if the JVM does not provide SHA-256
	 */
	public String hexDigest(String data) {
		// Input validation at the trust boundary: never hand a null to MessageDigest and
		// let a NullPointerException stack trace escape to the caller.
		if (data == null) {
			throw new IllegalArgumentException("data must not be null");
		}
		try {
			MessageDigest digest = MessageDigest.getInstance(ALGORITHM);
			// Encoding is pinned to UTF-8. Relying on the platform default charset makes
			// the same input hash differently on a different host -- a silent integrity
			// failure that is very hard to diagnose later.
			byte[] hash = digest.digest(data.getBytes(StandardCharsets.UTF_8));
			return HexFormat.of().formatHex(hash);
		} catch (NoSuchAlgorithmException e) {
			// SHA-256 is mandatory in every conformant JRE, so this is an environment
			// fault, not a user error. Fail closed rather than silently downgrading.
			throw new IllegalStateException(ALGORITHM + " is not available in this JVM", e);
		}
	}

	/**
	 * Constant-time comparison of a freshly computed digest against an expected value.
	 *
	 * <p>{@link String#equals(Object)} short-circuits on the first differing character,
	 * which leaks how much of a guess was correct through response timing. Verification
	 * therefore goes through {@link MessageDigest#isEqual(byte[], byte[])}.
	 *
	 * @param data             the text to hash
	 * @param expectedHexDigest the digest the file is supposed to have
	 * @return {@code true} only if the computed digest matches
	 */
	public boolean verify(String data, String expectedHexDigest) {
		if (expectedHexDigest == null) {
			return false;
		}
		byte[] actual = hexDigest(data).getBytes(StandardCharsets.US_ASCII);
		byte[] expected = expectedHexDigest.trim().toLowerCase().getBytes(StandardCharsets.US_ASCII);
		return MessageDigest.isEqual(actual, expected);
	}
}
