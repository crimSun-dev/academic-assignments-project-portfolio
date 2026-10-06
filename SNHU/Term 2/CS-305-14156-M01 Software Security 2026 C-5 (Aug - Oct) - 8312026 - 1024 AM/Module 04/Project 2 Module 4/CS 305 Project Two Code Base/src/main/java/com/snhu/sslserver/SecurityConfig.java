package com.snhu.sslserver;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.Customizer;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.header.writers.ReferrerPolicyHeaderWriter;
import org.springframework.security.web.header.writers.XXssProtectionHeaderWriter;

/**
 * HTTP-layer hardening.
 *
 * <p>TLS on the wire only protects data in transit. These headers tell the browser how to
 * treat the response once it arrives, which is where the remaining client-side attack
 * surface lives. Defence in depth: each header catches something the others do not.
 */
@Configuration
@EnableWebSecurity
public class SecurityConfig {

	@Bean
	public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
		http
			// /hash is the public verification route the client is required to reach in a
			// browser, so it is deliberately anonymous. Every other path stays closed, so
			// a future endpoint is unreachable until someone consciously opens it
			// (fail-closed default, A01:2021 - Broken Access Control).
			.authorizeHttpRequests(auth -> auth
				.requestMatchers("/hash").permitAll()
				.anyRequest().authenticated())
			.httpBasic(Customizer.withDefaults())

			// No server-side session is created, so there is no session to fix or steal.
			// That also makes CSRF protection unnecessary for this stateless, read-only
			// GET endpoint -- disabled deliberately, not by oversight.
			.csrf(csrf -> csrf.disable())

			.headers(headers -> headers
				// HSTS: after the first visit the browser refuses to speak plaintext to
				// this host, which closes the SSL-strip downgrade window.
				.httpStrictTransportSecurity(hsts -> hsts
					.includeSubDomains(true)
					.maxAgeInSeconds(31536000))
				// CSP: the response is self-contained HTML with no scripts. Saying so
				// explicitly means an injected <script> has no permitted origin to load
				// from even if output encoding is ever missed.
				.contentSecurityPolicy(csp -> csp
					.policyDirectives("default-src 'none'; style-src 'self'; frame-ancestors 'none'"))
				// Stops MIME sniffing turning a text response into executable content.
				.contentTypeOptions(Customizer.withDefaults())
				// Clickjacking: the checksum page must never be framed.
				.frameOptions(frame -> frame.deny())
				// Keeps the query string (which may contain the data being verified) out
				// of the Referer header sent to third parties.
				.referrerPolicy(referrer -> referrer
					.policy(ReferrerPolicyHeaderWriter.ReferrerPolicy.NO_REFERRER))
				.xssProtection(xss -> xss
					.headerValue(XXssProtectionHeaderWriter.HeaderValue.ENABLED_MODE_BLOCK)));

		return http.build();
	}
}
