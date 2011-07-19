
Detection of near-duplicate documents
=====================================

A sample implementation of Charikar's hash for identification of similar
documents.

<https://github.com/vilda/shash>


How to
------

The calculation of the hash is performend in the following way:

* Document is splitted into tokens (words for example) or super-tokens (word tuples)
* Each token is represented by its hash value; a traditonal hash function is used
* Weights are associated with tokens
* A vector `V` of integers is initialized to `0`, length of the vector corresponds to the desired hash size in bits
* In a cycle for all token's hash values (`h`), vector `V` is updated:
  * i<sup>th</sup> element is decreased by token's weight if the i<sup>th</sup> bit of the hash `h` is `0`, otherwise
  * i<sup>th</sup> element is increased by token's weight if the i<sup>th</sup> bit of the hash `h` is `1`
* Finally, signs of elements of `V` corresponds to the bits of the final fingerprint


References
----------

* Charikar: Similarity Estimation Techniques from Rounding Algorithms, in Proceedings of the thiry-fourth annual ACM symposium on Theory of computing, ACM Press, 2002
* Manku, Jain, Sarma: Detecting Near-Duplicates for Web Crawling. in Proceedings of the 16th international conference on World Wide Web, ACM Press, 2007
* Henzinger: Finding Near-Duplicate Web Pages: A Large-Scale Evaluation of Algorithms, in Proceedings of the 29th annual international ACM SIGIR conference on Research and development in information retrieval, ACM Press, 2006

---
Viliam Holub <vilda@logentries.com>
