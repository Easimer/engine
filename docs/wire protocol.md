# Wire Protocol of engine

This protocol runs over UDP on port 38576. Data is encapsulated
using Google's flatbuffers: every packet has a header
`MessageHeader`. This headers has one type field, that specifies
what is in the payload.

This file only documents protocol behavior. All structures
and constants are defined in `/schemas/edict.fbs`.

## MessageHeader
| Type                | Value    | Description |
| ------------------- | --------:| ----------- |
| NONE                | 0        | A packet with this type is invalid
| CONNECT             | 1        | Connection request made by a client
| CONNECT_ACK         | 2        | Acknowledgement of connection request
| CONNECT_NAK         | 3        | Negative acknowledgement of connection request
| DISCONNECT          | 4        | Disconnect request by a client
| DISCOVERY_PROBE     | 5        | Discovery probe
| DISCOVERY_RESPONSE  | 6        | Discovery probe response
| ENTITY_UPDATE       | 7        | Entity update sent to clients
| CLIENT_UPDATE       | 8        | Client input sent to server
| ECHO_REQUEST        | 9        | Echo request sent to the server
| ECHO_REPLY          | 10       | Echo reply sent to as an answer to a request
| QUERY_REQUEST       | 11       | Server info query
| QUERY_REPLY         | 12       | Server info query response
| SYNC_CLOCK          | 13       | Server clock state broadcast

## NONE
Packets with this type must be discarded, but the
server MUST reset the client's timeout timer
(if the source address is in fact connected).

## CONNECT
Payload: _ConnectData_

Connection request coming from a client.
Contains their chosen username.
The server might reject the request for
reasons specified in the CONNECT_NAK
section.
If the request is accepted, then
the server must respond with a
CONNECT_NAK.

## CONNECT_ACK
Payload: _ConnectData_

A server must acknowledge a successful connection request.
`conn_id` must be set to a unique value identifying the
client.

## CONNECT_NAK
Payload: _ConnectData_

Server must reject a request with CONNECT_NAK, if:
* The client is already connected: the source
address:port pair is already owned by a connected
client. In practice this only happens when
a CONNECT_ACK/NAK has already been sent, but
the client haven't yet received any response
and sends duplicate CONNECTs.
[nak_reason := ALREADY_CONNECTED]
* The chosen username is unavailable.
[nak_reason := NAME_UNAVAILABLE]
* The server has reached it's maximum player limit.
[nak_reason := FULL]
* For any reason, provided that it sets the
nak_reason_custom field.
[nak_reason := CUSTOM]

## DISCONNECT

If either party receives a DISCONNECT packet,
they must stop all communication.

## DISCOVERY_PROBE

A broadcast emitted by a client. Any
server on the local subnet MAY respond
to this with a DISCOVERY_RESPOND. 
Used to discover LAN servers.

## DISCOVERY_RESPONSE
Payload: _ServerData_

A server's response to a DISCOVERY_PROBE,
that contains the server's human-readable
name, the current and maximal player count
and the name of the currently loaded
level.

## ENTITY_UPDATE
Payload: _EntityUpdate_

Contains update of an entity identified
by the id `edict_id`.
`pos` is the position, `rot` is the
rotation in 4x4 matrix format,
`model` is the name of the model to draw
and `last_update` is the time the entity
was updated.

## CLIENT_UPDATE

## ECHO_REQUEST
## ECHO_REPLY
## QUERY_REQUEST
## QUERY_REPLY
## SYNC_CLOCK
