# NewBCPL Compiler Investigations

This directory contains detailed investigation reports for significant bugs and issues found during the development of the NewBCPL compiler. Each investigation follows a systematic approach to identify root causes and provide actionable solutions.

## Investigation Files

### Active Cases

#### [Register String Empty Bug Investigation](register-string-empty-bug-investigation.md)
- **Case ID**: BCPL-2024-001
- **Status**: SOLVED ✅
- **Severity**: Critical
- **Issue**: "Register string cannot be empty" error during recursive function compilation
- **Root Cause**: Circular dependency between code generation and register allocation phases
- **Detective**: Chief Investigator AI

## Investigation Methodology

Our investigations follow a systematic approach:

1. **Crime Scene Analysis**: Document the exact symptoms and reproduction steps
2. **Initial Hypothesis**: Form theories based on available evidence
3. **Systematic Debugging**: Use strategic debug output to trace execution flow
4. **Root Cause Analysis**: Identify the fundamental architectural or logical flaw
5. **Solution Design**: Propose actionable fixes with clear implementation steps
6. **Documentation**: Create comprehensive reports for future reference

## Investigation Template

When creating new investigation reports, follow this structure:

```markdown
# Investigation Report: [Bug Title]

**Case ID**: BCPL-YYYY-XXX
**Detective**: [Investigator Name]
**Date**: [Investigation Date]
**Status**: [OPEN/IN_PROGRESS/SOLVED/CLOSED]
**Severity**: [Critical/High/Medium/Low]

## Executive Summary
[Brief overview of the issue and findings]

## The Crime Scene
[Detailed description of symptoms and reproduction steps]

## Investigation Timeline
[Chronological account of investigation phases]

## Root Cause Analysis
[Technical analysis of the fundamental problem]

## Recommended Solution
[Actionable fix recommendations]

## Case Status
[Current status and next actions]
```

## Key Lessons Learned

1. **Systematic Debugging Beats Clever Theories**: Methodical trace execution is more reliable than assumptions
2. **Architectural Issues Are Often Hidden**: Complex bugs frequently stem from fundamental design flaws
3. **Document Everything**: Detailed investigation reports prevent future duplicate work
4. **Follow the Evidence**: Debug output placement is crucial for pinpointing exact failure locations

## Contributing to Investigations

When investigating new bugs:

1. Create a new investigation file using the template above
2. Use systematic debugging approaches rather than guesswork
3. Document all evidence, even if it leads to dead ends
4. Focus on root causes rather than symptoms
5. Provide clear, actionable solutions

## Archive Policy

- **Active Cases**: Currently being investigated
- **Solved Cases**: Root cause identified, solution implemented
- **Closed Cases**: Solved cases that have been verified fixed in production

---

*"Every bug is a mystery waiting to be solved. The key is to follow the evidence, not our assumptions."*