package com.snhu.sslserver;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

/**
 * CS 305 Module Five Coding Assignment: Checksum Verification
 *
 * Author: Draven Chen
 *
 * Boots an HTTPS (TLS) Spring Boot server that exposes a RESTful route which
 * generates a SHA-256 checksum for a unique data string.
 */
@SpringBootApplication
public class ServerApplication {

	public static void main(String[] args) {
		SpringApplication.run(ServerApplication.class, args);
	}

}

@RestController
class ServerController {

	/** Collision-resistant hash selected from the Java Security Standard Algorithm Names. */
	private static final String ALGORITHM = "SHA-256";

	/** Unique data string containing my first and last name. */
	private static final String DATA = "Hello Draven Chen!";

	/**
	 * RESTful route that returns the unique data string, the name of the algorithm
	 * cipher used, and the hexadecimal checksum (hash) value of the data string.
	 */
	@RequestMapping("/hash")
	public String myHash() {
		try {
			// 1. Create a MessageDigest object using the java.security.MessageDigest library
			//    and initialize it with the selected algorithm cipher.
			MessageDigest md = MessageDigest.getInstance(ALGORITHM);

			// 2. Use digest() to generate the hash value as a byte array from the data string.
			byte[] digest = md.digest(DATA.getBytes(StandardCharsets.UTF_8));

			// 3. Convert the byte array to a readable hexadecimal string.
			String checksum = bytesToHex(digest);

			// 4. Return the required information to the secure web browser.
			return "<p>Name / Unique Data String: " + DATA + "</p>"
					+ "<p>Algorithm Cipher: " + ALGORITHM + "</p>"
					+ "<p>Checksum Hash Value (hex): " + checksum + "</p>";
		} catch (NoSuchAlgorithmException e) {
			// Thrown only if the JVM does not provide the requested algorithm.
			return "<p>Error: the algorithm cipher \"" + ALGORITHM + "\" is not available.</p>";
		}
	}

	/**
	 * Converts a byte array produced by MessageDigest.digest() into a lowercase
	 * hexadecimal string. Each byte becomes exactly two hex characters, so a
	 * 256-bit SHA-256 digest renders as 64 characters.
	 */
	private static String bytesToHex(byte[] hash) {
		StringBuilder hexString = new StringBuilder(2 * hash.length);
		for (byte b : hash) {
			String hex = Integer.toHexString(0xff & b);
			if (hex.length() == 1) {
				hexString.append('0');
			}
			hexString.append(hex);
		}
		return hexString.toString();
	}
}
