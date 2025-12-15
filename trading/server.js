
API_SERVER="https://phisix-api3.appspot.com/"

import Alpaca from "@alpacahq/alpaca-trade-api";

// Alpaca() requires the API key and sectret to be set, even for crypto
const alpaca = new Alpaca({
  keyId: "PKL7284AY18RKMWQF6S9",
  secretKey: "HiWLJBRlm3wC3PlyZslkyhgjZSkEhCZWbgfKaS14",
});