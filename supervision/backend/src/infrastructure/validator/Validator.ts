import { FieldNotSpecifiedError } from "./exceptions/FieldNotSpecifiedError";
import { validate as uuidValidate } from 'uuid';

/**
 * Classe pour gérer les erreurs de validation.
 * Utilise une Map pour stocker les erreurs par champ.
 */
export class ValidatorErrorMap {
	private errors: Map<string, string[]> = new Map();

	public set(fieldName: string, error: string) {
		if (this.errors.has(fieldName)) {
			this.errors.get(fieldName)!.push(error);
		} else {
			this.errors.set(fieldName, [error]);
		}
	}

	public getErrors(): any {
		return Object.fromEntries(this.errors);
	}

	public hasErrors(): boolean {
		return this.errors.size > 0;
	}
}
/**
 * Callback pour la validation des éléments d'un tableau.
 * Permet de définir des règles de validation pour chaque élément.
 * @param itemValidator Le validateur pour l'élément courant.
 */
type ArrayItemValidatorCallback = (itemValidator: Validator<any>) => void;

/**
 * Callback pour les conditions de validation.
 * Permet de définir des règles conditionnelles.
 * @param object L'objet en cours de validation.
 */
type ConditionCallback<T> = (object: T) => boolean;


/**
 * Classe principale de validation.
 * Permet de chaîner les validations sur les champs d'un objet.
 * Utilise une instance de ValidatorErrorMap pour stocker les erreurs.
 * @param T Le type de l'objet à valider.
 * @example
 * const validator = new Validator<User>({ name: "John", email: "invalid-email" });
 * validator.field("name").isRequired().minLength(3);
 * validator.field("email").isRequired().isEmail();
 */
export class Validator<T> {
	private errors = new ValidatorErrorMap();
	private object: T;
	private currentField: keyof T | null = null;
	private conditionCallback: ConditionCallback<T> | null = null;

	// This is used to prevent multiple error messages for the same field
	// avoiding computing errors if previous predicate failed
	// todo: think about it
	//private previousCheckFailed: boolean = false;

	constructor(object: T) {
		this.object = object;
	}

	/**
	 * Définit le champ courant à valider.
	 * @param field Le champ de l'objet à valider.
	 * @returns L'instance du validateur pour chaîner les validations.
	 */
	public field<K extends keyof T>(field: K): Validator<T> {
		this.currentField = field;
		this.conditionCallback = null; // Reset condition callback for the new field
		return this;
	}

	/**
	 * Applique une condition pour les validations suivantes.
	 * Si la condition n'est pas remplie, les validations suivantes sont ignorées.
	 * @param condition La condition à vérifier.
	 * @returns L'instance du validateur pour chaîner les validations.
	 */
	public when(condition: (object: T) => boolean): Validator<T> {
		this.conditionCallback = () => condition(this.object);
		return this;
	}

	/**
	 * Vérifie que le champ courant est défini.
	 * Lance une erreur si aucun champ n'est défini.
	 * @throws FieldNotSpecifiedError si aucun champ n'est défini.
	 */
	public ensureFieldSet() {
		if (this.currentField === null) {
			throw new FieldNotSpecifiedError();
		}
	}

	/**
	 * Vérifie que le champ courant est requis.
	 * @note Si isRequired() n'est pas appelé, les autres validations sont ignorées si le champ est null ou undefined.
	 * @returns L'instance du validateur pour chaîner les validations.
	 */
	public isRequired(): Validator<T> {
		this.ensureFieldSet();

		if (this._isConditionSkipped()) {
		return this;
		}

		const value = this.object[this.currentField!];
		if (value === undefined || value === null || value === '') {
			this.errors.set(this.currentField as string, 'This field is required');
		}
		return this;
	}

	/**
	 * Vérifie que le champ courant est un email valide.
	 * @returns L'instance du validateur pour chaîner les validations.
	 */
	public isEmail(): Validator<T> {
		if(this._isNull()) return this;

		const value = this.object[this.currentField!];
		const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
		if (typeof value !== 'string' || !emailPattern.test(value)) {
			this.errors.set(this.currentField as string, 'Invalid email format');
		}
		return this;
	}

	/**
	 * Vérifie que le champ courant a une longueur minimale.
	 * @param length Le longueur minimale requise.
	 * @returns L'instance du validateur pour chaîner les validations.
	 * @note Peut agir sur les types string et array.
	 */
	public minLength(length: number): Validator<T> {
		if (this._isNull()) return this;

		const value = this.object[this.currentField!];

		if ((typeof value === 'string' || Array.isArray(value)) && value.length < length) {
			this.errors.set(this.currentField as string, `Minimum length is ${length}`);
		}

		return this;
	}

	public maxLength(length: number): Validator<T> {
		if (this._isNull()) return this;

		const value = this.object[this.currentField!];

		if ((typeof value === 'string' || Array.isArray(value)) && value.length > length) {
			this.errors.set(this.currentField as string, `Maximum length is ${length}`);
		}

		return this;
	}

	public isNumber(): Validator<T> {
		if(this._isNull()) return this;
		const value = this.object[this.currentField!];
		if (typeof value !== 'number') {
			this.errors.set(this.currentField as string, 'This field must be a number');
		}
		if(isNaN(value as number)) {
			this.errors.set(this.currentField as string, 'This field must be a number');
		}
		return this;
	}

	public isDate(): Validator<T> {
		if(this._isNull()) return this;

		const value = this.object[this.currentField!];
		if (!(value instanceof Date)) {
			this.errors.set(this.currentField as string, 'This field must be a date');
		}
		return this;
	}

	public isBoolean(): Validator<T> {
		if(this._isNull()) return this

		const value = this.object[this.currentField!];
		if (typeof value !== 'boolean') {
			this.errors.set(this.currentField as string, 'This field must be a boolean');
		}
		return this;
	}

	public greaterThan(value: number): Validator<T> {
		if(this._isNull()) return this;

		const fieldValue = this.object[this.currentField!];
		if (typeof fieldValue !== 'number' || fieldValue <= value) {
			this.errors.set(this.currentField as string, `This field must be greater than ${value}`);
		}
		return this;
	}

	public isUUID(): Validator<T> {
		if(this._isNull()) return this;
		const value = this.object[this.currentField!];
		if( typeof value !== 'string' || !uuidValidate(value)) {
			this.errors.set(this.currentField as string, 'This field must be a valid UUID');
		}
		return this;
	}
	
	public lessThan(value: number): Validator<T> {
		if(this._isNull()) return this;

		const fieldValue = this.object[this.currentField!];
		if (typeof fieldValue !== 'number' || fieldValue >= value) {
			this.errors.set(this.currentField as string, `This field must be less than ${value}`);
		}
		return this;
	}

	public matches(regex: RegExp, requirementDescription: string | null): Validator<T> {
		if(this._isNull()) return this;

		const value = this.object[this.currentField!];
		if (typeof value !== 'string' || !regex.test(value)) {
			this.errors.set(this.currentField as string, requirementDescription || 'This field does not meet the requirements');
		}

		return this;
	}

	public inEnum(enumObject: object): Validator<T> {
		if(this._isNull()) return this;
		const value = this.object[this.currentField!];
		const enumValues = Object.values(enumObject);
		if (!enumValues.includes(value)) {
			this.errors.set(this.currentField as string, `This field must be one of: ${enumValues.join(', ')}`);
		}
		return this;
	}

	public isArray<U>(): Validator<T> {
		if(this._isNull()) return this;

		const value = this.object[this.currentField!];
		if (!Array.isArray(value)) {
			this.errors.set(this.currentField as string, 'This field must be an array');
		}
		return this;
	}

	public forEach(callback: ArrayItemValidatorCallback) : Validator<T> {
		this.ensureFieldSet();
		const value = this.object[this.currentField!];

		if (!Array.isArray(value)) {
			this.errors.set(this.currentField as string, "This field must be an array");
			return this;
		}

		for (const item of value) {
			const subValidator = new Validator<{ item: any }>({ item });
			subValidator.field("item");
			callback(subValidator);

			if (subValidator.hasErrors()) {
				this.errors.set(this.currentField as string, `Item Error: ${JSON.stringify(subValidator.getErrors())}`);
				break;
			}
		}

		return this;
	}

	public customCheck(checkFunction: (value: any) => boolean, errorMessage: string): Validator<T> {
		if(this._isNull()) return this;

		const isValid = checkFunction(this.object[this.currentField!]);
		if (!isValid) {
			this.errors.set(this.currentField as string, errorMessage);
		}
		return this;
	}

	public getErrors(): any {
		return this.errors.getErrors();
	}

	public hasErrors(): boolean {
		return this.errors.hasErrors();
	}

	private _isConditionSkipped(): boolean {
		return this.conditionCallback !== null && !this.conditionCallback(this.object);
	}

	// Note: It is important to ignore when the value is null
	// because if we don't it will be always the same as isRequired().XXX() 
	// because null is not number, string etc.
	private _isNull(): boolean{
		this.ensureFieldSet();
		const value = this.object[this.currentField!];

		if (this._isConditionSkipped()) {
			return true; // Skip validation if condition is not met
		}

		return (value === null || value === undefined);
	}
}