const CACHE_NAME = 'me.aljami.game.sokoban-v1';
const PRECACHE_ASSETS = [
    './',
    './index.html',
    './favicon.ico',
    './icon.png',
    './icon620x620.png',
    './sokoban.js',
    './sokoban.wasm',
    './sokoban.data',
];

self.addEventListener('install', event => {
    event.waitUntil((async () => {
        const cache = await caches.open(CACHE_NAME);
        cache.addAll(PRECACHE_ASSETS);
    })());
});

self.addEventListener('activate', event => {
    event.waitUntil(self.clients.claim());
});

self.addEventListener('fetch', event => {
    event.respondWith(async () => {
        const cache = await caches.open(CACHE_NAME);

        // match the request to our cache
        const cachedResponse = await cache.match(event.request);

        // check if we got a valid response
        if (cachedResponse !== undefined) {
            // Cache hit, return the resource
            return cachedResponse;
        } else {
            // Otherwise, go to the network
            return fetch(event.request)
        };
    });
});