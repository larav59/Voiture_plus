import { createAuthenticatedDecorator } from "../../infrastructure/authentication/Authenticated";
import { Request } from "express";
import { UserIdentity } from "./UserIdentity";
import { jwtConfig } from "../../config";
import jwt from "jsonwebtoken";



export const Authenticated = createAuthenticatedDecorator<UserIdentity>({
	extractIdentity: (req: Request) => {
		const token = req.headers.authorization?.split(" ")[1];
		//if (!token) return null;

		//const payload = jwt.verify(token, jwtConfig.secret) as any;
		return new UserIdentity("1");
		//return UserIdentity.fromPayload(payload);
	},
	createError: () => new Error("Vous devez être authentifié pour accéder à cette ressource")
});
