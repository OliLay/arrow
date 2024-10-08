% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/duckdb.R
\name{to_arrow}
\alias{to_arrow}
\title{Create an Arrow object from a DuckDB connection}
\usage{
to_arrow(.data)
}
\arguments{
\item{.data}{the object to be converted}
}
\value{
A \code{RecordBatchReader}.
}
\description{
This can be used in pipelines that pass data back and forth between Arrow and
DuckDB.
}
\details{
Note that you can only call \code{collect()} or \code{compute()} on the result of this
function once. To work around this limitation, you should either only call
\code{collect()} as the final step in a pipeline or call \code{as_arrow_table()} on the
result to materialize the entire Table in-memory.
}
\examples{
\dontshow{if (getFromNamespace("run_duckdb_examples", "arrow")()) (if (getRversion() >= "3.4") withAutoprint else force)(\{ # examplesIf}
library(dplyr)

ds <- InMemoryDataset$create(mtcars)

ds \%>\%
  filter(mpg < 30) \%>\%
  to_duckdb() \%>\%
  group_by(cyl) \%>\%
  summarize(mean_mpg = mean(mpg, na.rm = TRUE)) \%>\%
  to_arrow() \%>\%
  collect()
\dontshow{\}) # examplesIf}
}
