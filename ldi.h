/* Handle lcp-delimited intervals, which map to unique substrings.
 * See "Using Suffix Arrays to Compute Term Frequency and Document
 * Frequency for All Substrings in a Corpus" by Mikio Yamamoto and
 * Kenneth W. Churcht. */
typedef void (*ldi_cb_t)(const char *str, unsigned len, unsigned count);
void ldi_walk(ldi_cb_t cb);
