# Compliance Check Tool

## Purpose

This tool area is reserved for lightweight checks that validate repository state against documented compliance expectations.

## What Belongs Here

* Documentation or scripts that compare implementation artifacts to control requirements.
* Checks for evidence completeness, interface inventory updates, or mandatory document presence.
* Reporting formats suitable for human review.

## What Must Not Be Placed Here

* False automation that claims final compliance certification.
* Production secrets or customer data.
* Heavy dependencies that obscure what the tool is verifying.

## Security and Compliance Notes

Checks here should highlight missing evidence, undocumented interfaces, or unreviewed security changes, but they must not overstate the project’s compliance status.

## Cross-Platform Notes

Prefer repository-level checks that remain valid across all device platforms and deployment targets.
