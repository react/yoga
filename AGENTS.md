# Yoga

Yoga is the cross-platform layout engine used by React Native. Follow
[`CONTRIBUTING.md`](CONTRIBUTING.md) when preparing a pull request.

## Changelog entries

React Native includes Yoga changes in its releases. In your pull request
description, include a React Native changelog entry so a user-visible fix can
appear in its release notes. Describe the effect on layouts or the affected API,
not just the implementation. For example:

```text
Changelog:
[General][Fixed] - Fix Yoga flex shrink producing oversized items when all children reach their minimum sizes
```

Use `[General]` for cross-platform Yoga behavior; use `[Android]` or `[iOS]`
only when the change affects just that platform. Choose the appropriate type:
`[Breaking]`, `[Added]`, `[Changed]`, `[Deprecated]`, `[Removed]`, `[Fixed]`, or
`[Security]`. If the change truly has no user-visible effect, such as a
test-only or documentation change, use `Changelog: [Internal]` instead.

See the [React Native changelog guidelines](https://reactnative.dev/contributing/changelogs-in-pull-requests)
for the complete format.
