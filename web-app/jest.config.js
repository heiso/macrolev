// @ts-check

/**
 * @type {import('@jest/types').Config.InitialOptions}
 */
const config = {
  // extensionsToTreatAsEsm: ['.ts'],
  // collectCoverage: true,
  // coverageReporters: ['lcov', 'text-summary'],
  projects: [
    {
      transform: {
        '^.+\\.(t|j)sx?$': '@swc/jest',
      },
      testEnvironment: 'node',
      clearMocks: true,
      testMatch: ['<rootDir>/**/*.spec.ts'],
      // collectCoverageFrom: ['<rootDir>/src/**/*.ts'],
      rootDir: './',
      // setupFilesAfterEnv: ['<rootDir>/jest.setup.ts'],
    },
  ],
}
export default config
