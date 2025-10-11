import { Validator } from "../../infrastructure/validator/Validator";

export abstract class BaseRequest<T> {
	
	// Méthode pour la validation
	public abstract validate(): Validator<T>;

	// Vérifier si la requête est valide
	public isValid(): boolean {
		return !this.validate().hasErrors()
	}

	// Récupérer les erreurs de validation
	public getErrors(): string[] {
		return this.validate().getErrors();
	}

}