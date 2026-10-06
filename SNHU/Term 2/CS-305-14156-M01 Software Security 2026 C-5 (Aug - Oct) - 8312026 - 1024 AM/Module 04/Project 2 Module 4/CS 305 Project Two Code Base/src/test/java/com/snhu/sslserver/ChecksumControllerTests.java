package com.snhu.sslserver;

import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.content;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.header;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;

import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.webmvc.test.autoconfigure.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.TestPropertySource;
import org.springframework.test.web.servlet.MockMvc;

/**
 * Security regression tests for the /hash route.
 *
 * <p>Each test here corresponds to a vulnerability that was designed out during the
 * refactor. They exist so that a future change cannot quietly reintroduce one.
 */
@SpringBootTest
@AutoConfigureMockMvc
// TLS is terminated by the container, not by MockMvc, so the keystore is not loaded here.
@TestPropertySource(properties = { "server.ssl.enabled=false" })
class ChecksumControllerTests {

	@Autowired
	private MockMvc mockMvc;

	@Test
	@DisplayName("/hash is publicly reachable and reports name, algorithm and digest")
	void hashEndpointReturnsChecksum() throws Exception {
		mockMvc.perform(get("/hash"))
				.andExpect(status().isOk())
				.andExpect(content().string(org.hamcrest.Matchers.containsString("SHA-256")))
				.andExpect(content().string(org.hamcrest.Matchers.containsString("Checksum")))
				// 64 hex characters somewhere in the body == a real 256-bit digest
				.andExpect(content().string(org.hamcrest.Matchers.matchesPattern("(?s).*[0-9a-f]{64}.*")));
	}

	@Test
	@DisplayName("REGRESSION: reflected input is HTML-escaped, so /hash?data=<script> cannot fire")
	void reflectedInputIsEscaped() throws Exception {
		mockMvc.perform(get("/hash").param("data", "<script>alert(1)</script>"))
				.andExpect(status().isOk())
				.andExpect(content().string(org.hamcrest.Matchers.not(
						org.hamcrest.Matchers.containsString("<script>alert(1)</script>"))))
				.andExpect(content().string(org.hamcrest.Matchers.containsString("&lt;script&gt;")));
	}

	@Test
	@DisplayName("REGRESSION: hardening headers are present on every response")
	void securityHeadersPresent() throws Exception {
		mockMvc.perform(get("/hash"))
				.andExpect(header().string("X-Content-Type-Options", "nosniff"))
				.andExpect(header().string("X-Frame-Options", "DENY"))
				.andExpect(header().string("Referrer-Policy", "no-referrer"))
				.andExpect(header().exists("Content-Security-Policy"));
	}

	@Test
	@DisplayName("REGRESSION: undeclared routes are denied by default, not served")
	void undeclaredRoutesAreNotOpen() throws Exception {
		mockMvc.perform(get("/admin"))
				.andExpect(status().is(org.hamcrest.Matchers.greaterThanOrEqualTo(401)));
	}
}
