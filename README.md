# API Routes Overview
The ZCash API provides a comprehensive set of endpoints tailored for interacting with ZCash blockchain data, facilitating both simple queries and complex data retrieval operations. Each route is meticulously designed to cater to specific data needs, ensuring efficient and effective access to blockchain information.

## General Routes

**/hello**: A simple endpoint to verify the API's operational status, returning a welcoming message to the caller.

## Block Information
**/blocks**: Offers paginated access to blocks, respectively, enabling efficient data retrieval by limiting the number of items per request and supporting reverse ordering (Pagination Support)

**/blocks/all**: Fetches comprehensive data for all blocks within the blockchain, supporting both GET for data retrieval and OPTIONS for CORS preflight checks.

**/block/<string>**: Retrieves detailed information for a specific block identified by its hash.

## Transaction Information
**/transaction/<string>**: Provides detailed data for a specific transaction, accessible via the transaction's unique hash.

**/transactions**: Offers paginated access to transactions, respectively, enabling efficient data retrieval by limiting the number of items per request and supporting reverse ordering (Pagination Support)

**/transactions/all**: Allows for the retrieval of all transactions recorded within the blockchain.

**/transactions/details**: A POST endpoint designed for fetching detailed information of multiple transactions, based on an array of transaction IDs provided in the request body.

**/transaction/outputs/<string>** and **/transaction/inputs/<string>**: Fetch transparent outputs and inputs related to a specific transaction, aiding in the detailed analysis of transaction flows.

## Blockchain Analytics

**/blocks/total** and **/transactions/total**: Endpoints dedicated to providing aggregate counts of blocks and transactions, facilitating a high-level overview of blockchain activity.

**/metrics/transactions**: Offers insights into transaction metrics over a specified period, enhancing the analytical capabilities of the API.
Blockchain and Peer Information

**/chain**: Delivers general information about the blockchain's current state.
/peers/details: Provides data on network peers, contributing to a comprehensive understanding of the network's topology.

## Search Functionality
**/search**: A versatile POST endpoint designed for direct search operations within the blockchain data, supporting complex queries based on various parameters.
