# Development-document disposition

Date: 2026-09-03

This inventory covers the files that were directly under `dev-docs/` before
the 2026-09-03 archive sweep; files that were already under
`dev-docs/history/` are excluded. Rows retain the original basename after a
move so the sweep remains auditable.

- `complete`: the work has landed or shipped and the development artifact is a
  candidate for `dev-docs/history/`.
- `in progress`: the document belongs to active design, implementation, review,
  or release work and stays at the top level.
- `future`: the work is parked, deferred, or an open backlog item and stays at
  the top level.
- `superseded`: a later document, revision, or product direction replaced it;
  it is a candidate for `dev-docs/history/`.
- `process`: standing process, checkpoint, or program-management material and
  stays at the top level.

Archive sweep applied 2026-09-03: 274 eligible files moved to
`dev-docs/history/` (268 tracked files with `git mv`, plus 6 previously
untracked files). Forty `complete` or `superseded` files remain directly under
`dev-docs/` because the standing rulebook, a checker, or CI still names them.
All `in progress`, `future`, and `process` files also remain directly under
`dev-docs/`.

## Retained archive exceptions

| Filename | Reason retained |
| --- | --- |
| `GwzConfIntegrity-Review.md` | checker/CI |
| `GwzDevCodeM2a-Rem2ReviewPrompt.md` | standing rulebook |
| `GwzDevCodeM2a-RemPlan.md` | standing rulebook |
| `GwzDevCodeM2aReviewPriompt.md` | standing rulebook |
| `GwzM5-8DR1-Reconciliation-Design.md` | checker/CI |
| `GwzM5-8DR1-WarnOrRefuse-Charter.md` | checker/CI |
| `GwzM5-8I1DirectionMemo.md` | standing rulebook |
| `GwzM5-8I2ActionJournalContract.md` | standing rulebook and checker/CI |
| `GwzM5-8I2CompatibilityContract.md` | standing rulebook and checker/CI |
| `GwzM5-8I2ProtocolContract.md` | standing rulebook |
| `GwzM5-8I2RecordContract.md` | standing rulebook and checker/CI |
| `GwzM5-8M5bNoFfDesign.md` | checker/CI |
| `GwzM5-8R0Inventory.md` | standing rulebook |
| `GwzM5-8R1Ownership.md` | standing rulebook |
| `GwzM5-8R2C0Interface-ReviewCode-2.md` | standing rulebook |
| `GwzM5-8R2C0Interface-ReviewCode-3.md` | standing rulebook |
| `GwzM5-8R2C2OwnerInterface-ReviewCode-2.md` | standing rulebook |
| `GwzM5-8R2C2OwnerInterface-ReviewCode.md` | standing rulebook |
| `GwzM5-8R2C2OwnerInterface-ReviewState.md` | standing rulebook |
| `GwzM5-8R2CCatalogBootstrapAmendment.md` | standing rulebook |
| `GwzM5-8R2D-Plan.md` | checker/CI |
| `GwzM5-8R2DInterfaceFreeze.md` | checker/CI |
| `GwzM5-8R2DSettledTuple.md` | checker/CI |
| `GwzM5-8R2E-CapabilityFreeAmendment.md` | checker/CI |
| `GwzM5-8R2E-E6-Review.md` | checker/CI |
| `GwzM5-8R2E-E7-Acceptance.md` | checker/CI |
| `GwzM5-8R2E-RecordRootAmendment.md` | checker/CI |
| `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` | checker/CI |
| `GwzM5-8R2F-EvidenceMap.md` | checker/CI |
| `GwzM5-8R2F-RelocationPlan.md` | checker/CI |
| `GwzM5-8R2aOwnership.md` | standing rulebook |
| `GwzM5-8R4bG-Evidence.md` | checker/CI |
| `GwzM5-8R4bP1P2-RemPlan-4.md` | standing rulebook |
| `GwzM5-8R4bR1InterfaceCheckpoint.md` | standing rulebook |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` | standing rulebook |
| `GwzM5-8R4bR2ConsumerCheckpoint.md` | standing rulebook |
| `GwzM5-8R4bTransitionDesign.md` | standing rulebook and checker/CI |
| `GwzM5-8R4bTransitionImplementation-ReviewState.md` | standing rulebook |
| `GwzMergeCheckpoint-v0.10.5.md` | standing rulebook |
| `GwzReleaseReview-v0.10.5-F5.md` | standing rulebook |

## Disposition inventory

| Filename | Disposition |
| --- | --- |
| `AgentProcessRules-ReviewF5.md` | process |
| `AgentProcessRules.md` | process |
| `AnnounceGwz.md` | superseded |
| `AnnounceGwzHn.md` | superseded |
| `CurrentProgramCheckpoint.md` | process |
| `DevDocsDisposition.md` | process |
| `GwxMergeFindingsDraft.md` | process |
| `GwzAiReqDd-AdversarialReview.md` | future |
| `GwzAiReqDd.md` | future |
| `GwzArm64Ebadf-Review.md` | complete |
| `GwzArm64EbadfDiagnosis.md` | complete |
| `GwzArmPreservationHandoffDiagnosis.md` | complete |
| `GwzConfIntegrity-Review-2.md` | complete |
| `GwzConfIntegrity-Review.md` | superseded |
| `GwzDevCodeM2a-Rem2ReviewPrompt.md` | superseded |
| `GwzDevCodeM2a-Rem3ReviewPrompt.md` | superseded |
| `GwzDevCodeM2a-RemPlan-2.md` | superseded |
| `GwzDevCodeM2a-RemPlan-3.md` | complete |
| `GwzDevCodeM2a-RemPlan.md` | superseded |
| `GwzDevCodeM2a-Review56-2.md` | superseded |
| `GwzDevCodeM2a-Review56-3.md` | superseded |
| `GwzDevCodeM2a-Review56-4.md` | superseded |
| `GwzDevCodeM2a-Review56-6.md` | complete |
| `GwzDevCodeM2a-Review56.md` | superseded |
| `GwzDevCodeM2a-ReviewF5-2.md` | superseded |
| `GwzDevCodeM2a-ReviewF5-3.md` | superseded |
| `GwzDevCodeM2a-ReviewF5-4.md` | superseded |
| `GwzDevCodeM2a-ReviewF5-5.md` | superseded |
| `GwzDevCodeM2a-ReviewF5-6.md` | complete |
| `GwzDevCodeM2a-ReviewF5.md` | superseded |
| `GwzDevCodeM2aReviewPriompt.md` | superseded |
| `GwzDevCodeM2b-RemPlan.md` | complete |
| `GwzDiffD0Protocol.md` | complete |
| `GwzDiffD4RenderSpike.md` | complete |
| `GwzDiffPlan.md` | complete |
| `GwzDryRunClassFix-Review.md` | complete |
| `GwzFasterProposal.md` | process |
| `GwzGodFileRefactorPlan.md` | complete |
| `GwzLocalClone-RemPlan-2.md` | superseded |
| `GwzLocalClone-RemPlan-3.md` | in progress |
| `GwzLocalClone-RemPlan.md` | superseded |
| `GwzLocalClone-ReviewConsistency-2.md` | superseded |
| `GwzLocalClone-ReviewConsistency-3.md` | in progress |
| `GwzLocalClone-ReviewConsistency.md` | superseded |
| `GwzLocalClone-ReviewSafety-2.md` | superseded |
| `GwzLocalClone-ReviewSafety-3.md` | in progress |
| `GwzLocalClone-ReviewSafety.md` | superseded |
| `GwzLocalCloneDesign.md` | in progress |
| `GwzM5-8A1Activation-ReviewCompleteness.md` | complete |
| `GwzM5-8A1Activation-ReviewSafety.md` | complete |
| `GwzM5-8A1ActivationPackage-Report.md` | complete |
| `GwzM5-8A1ActivationRecord.md` | complete |
| `GwzM5-8A1DecisionPacket.md` | complete |
| `GwzM5-8A1ReleasePlan.md` | complete |
| `GwzM5-8A1ReleaseR1-ReviewState.md` | complete |
| `GwzM5-8A1ReleaseR2-ReviewState.md` | complete |
| `GwzM5-8A1ReleaseR3-Review.md` | complete |
| `GwzM5-8A1ReleaseR4-Review.md` | complete |
| `GwzM5-8ChangeBudget.md` | process |
| `GwzM5-8D3Impl-ReviewCode.md` | complete |
| `GwzM5-8D3Impl-ReviewState.md` | complete |
| `GwzM5-8DR1-Charter.md` | complete |
| `GwzM5-8DR1-FilesystemIdentity-Design.md` | complete |
| `GwzM5-8DR1-FilesystemIdentity-ReviewCode.md` | complete |
| `GwzM5-8DR1-FilesystemIdentity-ReviewState.md` | complete |
| `GwzM5-8DR1-Reconciliation-Design.md` | complete |
| `GwzM5-8DR1-Reconciliation-ReviewCode.md` | complete |
| `GwzM5-8DR1-Reconciliation-ReviewState.md` | complete |
| `GwzM5-8DR1-WarnOrRefuse-Charter.md` | complete |
| `GwzM5-8DurableCursor-ReviewCode.md` | complete |
| `GwzM5-8DurableCursor-ReviewState.md` | complete |
| `GwzM5-8DurableCursorAmendment.md` | complete |
| `GwzM5-8ExactEvidenceAmendment-ReviewCode.md` | complete |
| `GwzM5-8ExactEvidenceAmendment-ReviewState.md` | complete |
| `GwzM5-8ExactEvidencePlatformAmendment.md` | complete |
| `GwzM5-8I1DirectionMemo.md` | complete |
| `GwzM5-8I2ActionJournalContract.md` | complete |
| `GwzM5-8I2CompatibilityContract.md` | complete |
| `GwzM5-8I2ProtocolContract.md` | complete |
| `GwzM5-8I2RecordContract.md` | complete |
| `GwzM5-8I2Refreeze-ReviewConsistency.md` | complete |
| `GwzM5-8M5bImpl-ReviewCode.md` | complete |
| `GwzM5-8M5bImpl-ReviewState.md` | complete |
| `GwzM5-8M5bImplSettled-Review.md` | complete |
| `GwzM5-8M5bNoFf-ReviewCode.md` | complete |
| `GwzM5-8M5bNoFf-ReviewState.md` | complete |
| `GwzM5-8M5bNoFfDesign.md` | complete |
| `GwzM5-8M5c-Charter.md` | superseded |
| `GwzM5-8M5d-Charter.md` | in progress |
| `GwzM5-8M5d-I2Amendment.md` | in progress |
| `GwzM5-8M5d-RemPlan-2.md` | superseded |
| `GwzM5-8M5d-RemPlan-3.md` | in progress |
| `GwzM5-8M5d-RemPlan.md` | superseded |
| `GwzM5-8M5d-Review.md` | in progress |
| `GwzM5-8M5d-ReviewConsistency-2.md` | superseded |
| `GwzM5-8M5d-ReviewConsistency-3.md` | superseded |
| `GwzM5-8M5d-ReviewConsistency-4.md` | in progress |
| `GwzM5-8M5d-ReviewConsistency.md` | superseded |
| `GwzM5-8M5d-ReviewImplementation.md` | in progress |
| `GwzM5-8M5d-ReviewSafety-2.md` | superseded |
| `GwzM5-8M5d-ReviewSafety-3.md` | superseded |
| `GwzM5-8M5d-ReviewSafety-4.md` | in progress |
| `GwzM5-8M5d-ReviewSafety.md` | superseded |
| `GwzM5-8OperatorEscapeAmendment-ReviewCode.md` | complete |
| `GwzM5-8OperatorEscapeAmendment-ReviewState.md` | complete |
| `GwzM5-8OperatorEscapeAmendment.md` | complete |
| `GwzM5-8OperatorEscapeDesign.md` | complete |
| `GwzM5-8PanicConversionP1P3-ReviewCode.md` | complete |
| `GwzM5-8PanicInvariantAudit.md` | complete |
| `GwzM5-8ProgressReviewF5.md` | complete |
| `GwzM5-8R0Inventory.md` | complete |
| `GwzM5-8R0Review-Compat-5.md` | superseded |
| `GwzM5-8R0Review-Compat-6.md` | complete |
| `GwzM5-8R0Review-Harness-5.md` | superseded |
| `GwzM5-8R0Review-Harness-6.md` | complete |
| `GwzM5-8R1Ownership.md` | complete |
| `GwzM5-8R2C0Interface-ReviewCode-2.md` | superseded |
| `GwzM5-8R2C0Interface-ReviewCode-3.md` | complete |
| `GwzM5-8R2C0Interface-ReviewCode.md` | superseded |
| `GwzM5-8R2C0Interface-ReviewState-2.md` | superseded |
| `GwzM5-8R2C0Interface-ReviewState-3.md` | complete |
| `GwzM5-8R2C0Interface-ReviewState.md` | superseded |
| `GwzM5-8R2C1AggregateClassifier-ReviewCode-2.md` | complete |
| `GwzM5-8R2C1AggregateClassifier-ReviewCode.md` | superseded |
| `GwzM5-8R2C1AggregateClassifier-ReviewState-2.md` | complete |
| `GwzM5-8R2C1AggregateClassifier-ReviewState.md` | superseded |
| `GwzM5-8R2C2DirentBarrier-ReviewState.md` | complete |
| `GwzM5-8R2C2OwnerInterface-ReviewCode-2.md` | superseded |
| `GwzM5-8R2C2OwnerInterface-ReviewCode-3.md` | superseded |
| `GwzM5-8R2C2OwnerInterface-ReviewCode-4.md` | complete |
| `GwzM5-8R2C2OwnerInterface-ReviewCode.md` | superseded |
| `GwzM5-8R2C2OwnerInterface-ReviewState-2.md` | superseded |
| `GwzM5-8R2C2OwnerInterface-ReviewState-3.md` | complete |
| `GwzM5-8R2C2OwnerInterface-ReviewState.md` | superseded |
| `GwzM5-8R2C2PublicationAudit.md` | complete |
| `GwzM5-8R2CCatalogBootstrapAmendment-ReviewCode.md` | complete |
| `GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md` | complete |
| `GwzM5-8R2CCatalogBootstrapAmendment.md` | complete |
| `GwzM5-8R2D-Plan.md` | complete |
| `GwzM5-8R2DInterfaceFreeze-ReviewCode.md` | complete |
| `GwzM5-8R2DInterfaceFreeze-ReviewState.md` | complete |
| `GwzM5-8R2DInterfaceFreeze.md` | complete |
| `GwzM5-8R2DPhase1-ReviewCode.md` | complete |
| `GwzM5-8R2DPhase1-ReviewState.md` | complete |
| `GwzM5-8R2DPhase4Closure.md` | complete |
| `GwzM5-8R2DSettled-ReviewCode.md` | complete |
| `GwzM5-8R2DSettled-ReviewState.md` | complete |
| `GwzM5-8R2DSettledTuple.md` | complete |
| `GwzM5-8R2DStep21-Review.md` | complete |
| `GwzM5-8R2DStep21-ReviewState.md` | complete |
| `GwzM5-8R2DStep22-Review.md` | complete |
| `GwzM5-8R2DStep23-Review.md` | complete |
| `GwzM5-8R2DStep24-Review.md` | complete |
| `GwzM5-8R2DStep31-Review.md` | complete |
| `GwzM5-8R2DStep31b-Review.md` | complete |
| `GwzM5-8R2DStep32-Review.md` | complete |
| `GwzM5-8R2DStep33-Review.md` | complete |
| `GwzM5-8R2DStep41-Review.md` | complete |
| `GwzM5-8R2DStep42-Review.md` | complete |
| `GwzM5-8R2DStep43-Review.md` | complete |
| `GwzM5-8R2E-CapabilityFreeAmendment-ReviewCode.md` | complete |
| `GwzM5-8R2E-CapabilityFreeAmendment-ReviewState.md` | complete |
| `GwzM5-8R2E-CapabilityFreeAmendment.md` | complete |
| `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md` | complete |
| `GwzM5-8R2E-E01ReachTraces.md` | complete |
| `GwzM5-8R2E-E03-ReviewCode.md` | complete |
| `GwzM5-8R2E-E03-ReviewState.md` | complete |
| `GwzM5-8R2E-E1-Review.md` | complete |
| `GwzM5-8R2E-E2-Review.md` | complete |
| `GwzM5-8R2E-E3-Review.md` | complete |
| `GwzM5-8R2E-E4-Close.md` | complete |
| `GwzM5-8R2E-E4.1-Review.md` | complete |
| `GwzM5-8R2E-E4.2-Review.md` | complete |
| `GwzM5-8R2E-E4.3B-Review.md` | complete |
| `GwzM5-8R2E-E4.4-6B-Review.md` | complete |
| `GwzM5-8R2E-E4.4-CharterPrep.md` | complete |
| `GwzM5-8R2E-E4.7-Review.md` | complete |
| `GwzM5-8R2E-E45-6B-CharterPrep.md` | complete |
| `GwzM5-8R2E-E45B-Report.md` | complete |
| `GwzM5-8R2E-E47-CharterPrep.md` | complete |
| `GwzM5-8R2E-E5-Review.md` | complete |
| `GwzM5-8R2E-E6-Review.md` | complete |
| `GwzM5-8R2E-E7-Acceptance.md` | complete |
| `GwzM5-8R2E-E7-ReviewCode.md` | complete |
| `GwzM5-8R2E-E7-ReviewState.md` | complete |
| `GwzM5-8R2E-GcArchiveDecode-Review.md` | complete |
| `GwzM5-8R2E-Plan.md` | in progress |
| `GwzM5-8R2E-RecordRootAmendment-ReviewCode.md` | complete |
| `GwzM5-8R2E-RecordRootAmendment-ReviewState.md` | complete |
| `GwzM5-8R2E-RecordRootAmendment.md` | complete |
| `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` | complete |
| `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` | complete |
| `GwzM5-8R2F-EvidenceMap.md` | complete |
| `GwzM5-8R2F-R1.1-Review.md` | complete |
| `GwzM5-8R2F-R1.2-Review.md` | complete |
| `GwzM5-8R2F-RelocationPlan-Review.md` | complete |
| `GwzM5-8R2F-RelocationPlan.md` | complete |
| `GwzM5-8R2F-RelocationTrace.md` | complete |
| `GwzM5-8R2aOwnership.md` | complete |
| `GwzM5-8R4bAcceptance-ReviewState.md` | complete |
| `GwzM5-8R4bExecution-ReviewArch.md` | complete |
| `GwzM5-8R4bExecution-ReviewState.md` | complete |
| `GwzM5-8R4bFinalization-ReviewArch.md` | complete |
| `GwzM5-8R4bFinalization-ReviewState.md` | complete |
| `GwzM5-8R4bG-Evidence.md` | complete |
| `GwzM5-8R4bG-EvidenceInventory.md` | complete |
| `GwzM5-8R4bG-ReviewCorrectness.md` | complete |
| `GwzM5-8R4bG-ReviewEvidence.md` | complete |
| `GwzM5-8R4bInterfaceAmendment-1.md` | complete |
| `GwzM5-8R4bInterfaceAmendment-2-ReviewArch.md` | complete |
| `GwzM5-8R4bInterfaceAmendment-2-ReviewState.md` | complete |
| `GwzM5-8R4bInterfaceAmendment-2.md` | complete |
| `GwzM5-8R4bInterfaceAmendment-ReviewArch.md` | complete |
| `GwzM5-8R4bInterfaceAmendment-ReviewState.md` | complete |
| `GwzM5-8R4bLinuxCapabilitySpike.md` | complete |
| `GwzM5-8R4bLinuxIdentityEvidence-Review-2.md` | complete |
| `GwzM5-8R4bLinuxIdentityEvidence-Review.md` | superseded |
| `GwzM5-8R4bLinuxIdentityEvidence-Run31536272593.json` | superseded |
| `GwzM5-8R4bLinuxIdentityEvidence.json` | complete |
| `GwzM5-8R4bP0Code-ReviewArch-2.md` | complete |
| `GwzM5-8R4bP0Code-ReviewArch.md` | superseded |
| `GwzM5-8R4bP0Code-ReviewState-2.md` | complete |
| `GwzM5-8R4bP0Code-ReviewState.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-2.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewFS-2.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewFS-3.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewFS-4.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewFS.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewState-2.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewState-3.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewState-4.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3-ReviewState.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-3.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-2.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-3.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-4.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-5.md` | complete |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-2.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-3.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-4.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-5.md` | complete |
| `GwzM5-8R4bP1P2-RemPlan-4-ReviewState.md` | superseded |
| `GwzM5-8R4bP1P2-RemPlan-4.md` | complete |
| `GwzM5-8R4bP1P2-RemPlan.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewArch-2.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewArch-3.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewArch-4.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewArch.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewFaults-2.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewFaults-3.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewFaults-4.md` | superseded |
| `GwzM5-8R4bP1P2-ReviewFaults.md` | superseded |
| `GwzM5-8R4bR1Interface-RemPlan.md` | complete |
| `GwzM5-8R4bR1Interface-ReviewFS-2.md` | superseded |
| `GwzM5-8R4bR1Interface-ReviewFS-3.md` | superseded |
| `GwzM5-8R4bR1Interface-ReviewFS-4.md` | complete |
| `GwzM5-8R4bR1Interface-ReviewFS.md` | superseded |
| `GwzM5-8R4bR1Interface-ReviewState-2.md` | superseded |
| `GwzM5-8R4bR1Interface-ReviewState-3.md` | superseded |
| `GwzM5-8R4bR1Interface-ReviewState-4.md` | complete |
| `GwzM5-8R4bR1Interface-ReviewState.md` | superseded |
| `GwzM5-8R4bR1InterfaceCheckpoint.md` | complete |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-10.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-11-P3.md` | complete |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-11.md` | complete |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-2.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-3.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-4.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-5.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-6.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-7.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-8.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS-9.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewFS.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-10.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-11-P3.md` | complete |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-11.md` | complete |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-2.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-3.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-4.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-5.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-6.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState-7.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewState.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` | complete |
| `GwzM5-8R4bR2ConsumerCheckpoint-ReviewFS.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint-ReviewState.md` | superseded |
| `GwzM5-8R4bR2ConsumerCheckpoint.md` | superseded |
| `GwzM5-8R4bReverseLifecycleInterface-ReviewArch-2.md` | complete |
| `GwzM5-8R4bReverseLifecycleInterface-ReviewArch.md` | superseded |
| `GwzM5-8R4bReverseLifecycleInterface-ReviewState-2.md` | complete |
| `GwzM5-8R4bReverseLifecycleInterface-ReviewState.md` | superseded |
| `GwzM5-8R4bReverseLifecycleInterface.md` | complete |
| `GwzM5-8R4bServiceInterface-ReviewArch.md` | complete |
| `GwzM5-8R4bServiceInterface-ReviewState.md` | complete |
| `GwzM5-8R4bStore-ReviewArch-2.md` | complete |
| `GwzM5-8R4bStore-ReviewArch.md` | superseded |
| `GwzM5-8R4bTransitionDesign-ReviewArch.md` | complete |
| `GwzM5-8R4bTransitionDesign-ReviewState.md` | complete |
| `GwzM5-8R4bTransitionDesign.md` | complete |
| `GwzM5-8R4bTransitionImplementation-ReviewArch.md` | complete |
| `GwzM5-8R4bTransitionImplementation-ReviewState.md` | complete |
| `GwzM5-8Refactor-Review-8.md` | in progress |
| `GwzM5-8Refactor-ReviewF5-2-2.md` | in progress |
| `GwzM5-8Refactor-ReviewF5.md` | superseded |
| `GwzM5-8Refactor.md` | in progress |
| `GwzM5-8T5CandidatePair.patch` | superseded |
| `GwzM5-8ThinA1Amendment-ReviewConsistency.md` | complete |
| `GwzM5-8ThinA1Amendment-ReviewSafety.md` | complete |
| `GwzM5-8ThinA1Amendment.md` | complete |
| `GwzM5-8V0ForgedActionGate-ReviewState.md` | complete |
| `GwzMergeCheckpoint-v0.10.5.md` | complete |
| `GwzMergeCheckpoint-v0.11.0.md` | complete |
| `GwzMergeCode-ReviewF5-2.md` | complete |
| `GwzMergeCode-ReviewF5.md` | superseded |
| `GwzMergeCodeRemPlan-1.md` | complete |
| `GwzMergeM4-RemPlan.md` | complete |
| `GwzMergeM4-RemReview.md` | complete |
| `GwzMergeWedgeRunbook-v0.md` | complete |
| `GwzOverClaimingCommitDiagnosis.md` | complete |
| `GwzParameterPlumbingAudit.md` | future |
| `GwzProcessAdoption-ReviewConsistency.md` | process |
| `GwzProcessAdoption-ReviewSafety.md` | process |
| `GwzProcessOptimization.md` | process |
| `GwzPyCliParityIssues.md` | future |
| `GwzReleaseNotes-v0.11.0.md` | complete |
| `GwzReleaseNotes-v0.13.0.md` | complete |
| `GwzReleaseNotes-v0.14.0.md` | in progress |
| `GwzReleaseReview-v0.10.5-F5.md` | complete |
| `GwzWinNamespaceAnchor-Diagnosis.md` | complete |
| `GwzWinNamespaceAnchor-Review.md` | complete |
| `GwzWindowsMatrix-Classification.md` | complete |
| `GwzWindowsMatrix-ExactEvidenceDiagnosis.md` | complete |
| `GwzWt-P0.1-Review.md` | superseded |
| `GwzWtDesign-Review.md` | superseded |
| `GwzWtDesign.md` | superseded |
| `GwzWtPlan.md` | superseded |
| `HN-AnnounceGWZ.md` | superseded |
| `HomeReadmesPlan.md` | complete |
| `RepoClonePlan-Review56s.md` | complete |
| `RepoClonePlan-ReviewF5.md` | complete |
| `RepoClonePlan.md` | complete |
| `UserDocsPlan.md` | complete |
