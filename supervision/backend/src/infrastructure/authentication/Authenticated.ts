import { Request } from "express";
import { Unauthorized } from "../webserver/errors/HttpError";

/**
 * Options pour le décorateur d'authentification.
 * T est le type de l'identité utilisateur.
 */
export interface AuthDecoratorOptions<T> {
	/**
	 * Fonction pour extraire l'identité depuis la requête.
	 * Peut retourner null si non authentifié.
	 */
	extractIdentity: (req: Request) => T | null;

	/**
	 * Optionnel : fonction pour créer l'erreur à lever
	 * si l'identité est absente. Par défaut : Error("Unauthorized")
	 */
	createError?: () => Error;
}

/**
 * Crée un décorateur d'authentification.
 * @param options Options pour le décorateur d'authentification.
 * @returns Le décorateur d'authentification.
 */
export function createAuthenticatedDecorator<T>(options: AuthDecoratorOptions<T>) {
	const { extractIdentity, createError } = options;

	return function AuthenticatedDecorator() {
		return function (target: any, propertyKey: string, descriptor: PropertyDescriptor) {
			const originalMethod = descriptor.value;

			descriptor.value = async function (...args: any[]) {
				const req = args[0] as Request;

				const identity = extractIdentity(req);
				if (!identity) {
					throw createError?.() ?? new Unauthorized("Invalid Token");
				}

				req.identity = identity;

				return await originalMethod.apply(this, args);
			};
		};
	};
}
