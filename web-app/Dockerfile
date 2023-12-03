# syntax=docker/dockerfile:1
ARG NODE_VERSION=20.6.1



# ===
# base node image
# ===
FROM node:${NODE_VERSION}-slim as base

RUN apt-get update -qq && \
    apt-get install -y openssl 

LABEL fly_launch_runtime="Remix"

WORKDIR /app

ENV NODE_ENV=production



# ===
# Build the app
# ===
FROM base as build

# Install packages needed to build node modules
RUN apt-get update -qq && \
    apt-get install -y python-is-python3 pkg-config build-essential 

# Install node modules
COPY --link package.json package-lock.json ./
RUN npm install --production=false

# Copy application code
COPY --link . .

# Generate Routes
RUN npx tsx generate-remix-routes.ts

# Generate SVGs
RUN npx tsx generate-svg-icons-sprite.ts

# Build application
RUN npm run build

# Remove development dependencies
RUN npm prune --production



# ===
# Final stage for app image
# ===
FROM base

# Copy built application
COPY --from=build /app /app

CMD node --max-old-space-size=256 build/server
