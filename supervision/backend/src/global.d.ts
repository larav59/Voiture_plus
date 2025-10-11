
// Extension of libraries types

import { Identity } from "./infrastructure/webserver/Identity";


declare global {
    namespace Express {
        interface Request {
            identity: any | null;
        }
    }
}
