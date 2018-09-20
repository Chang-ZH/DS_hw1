# Data Structures and Programming -- Assignment 1 #

## 1. Introduction ##

PageRank is an algorithm used by Google Search to rank websites in their search engine results. PageRank
was named after Larry Page, one of the founders of Google. PageRank is a way of measuring the importance
of website pages. According to Google:
PageRank works by counting the number and quality of links to a page to determine a rough estimate of
how important the website is. The underlying assumption is that more important websites are likely to
receive more links from other websites.
It is not the only algorithm used by Google to order search engine results, but it is the first algorithm that
was used by the company, and it is the best-known.

Quoting from the original Google paper, PageRank is defined like this:
We assume page A has pages T1, ..., Tn which point to it (i.e., are citations). The parameter d is a
damping factor which can be set between 0 and 1. We usually set d to 0.85. Also C(A) is defined as
the number of links going out of page A. The PageRank of a page A is given as follows:

`PR(A) = (1 - d) / N + d * ( PR(T1) / C(T1) + ... + PR(Tn) / C(Tn) ), where N is the total number of pages.`

Note that the PageRanks form a probability distribution over web pages, so the sum of all web pages'
PageRanks will be one.

## 2. Assignment ##

- **Page Rank list**

    Your program will take two arguments, the stopping difference and the "d" value, and compute the page
rank for all 500 pages, stopping when the cumulative diff for all 500 goes below that number (a real
number greater than zero) - note, be careful when debugging - for some settings of these two parameters
your program could loop forever.
Your output should be a le with a list of the pages in page rank order (highest to lowest), for each page
listing its outbranching (how many pages it connects to) and the page rank for that page (to 8 signicant
digits).

- **Reverse index**

    Your program should also produce a file which shows its reverse index. That is, for every word (note:
including commonly used ones) you should output the word followed by a list of the pages that include
that word (please alphabetize the word list, the pages can be produced in any order you choose - but each
page should only appear once!).

- **Search engine**

    Given the data structures containing the above, your search engine is quite simple. You take as input a list
of words and you output the top ten pages (sorted by page rank) that contain them (if less than 10, then
print all of them). When a single word is entered, just output the pages for that word. When multiple
words are entered, output two lists - the top ten pages with all the words and the top ten pages with any
of the words (multiple words do not effect the ranking here).

