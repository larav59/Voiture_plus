import { Column, Entity, OneToMany, PrimaryGeneratedColumn } from "typeorm";
import { States } from "./States";
import { Travels } from "./Travels";

@Entity("VEHICLES", { schema: "supervision" })
export class Vehicles {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("varchar", { name: "name", nullable: true, length: 255 })
  public name!: string | null;

  @OneToMany(() => States, (states) => states.vehicle)
  public states!: States[];

  @OneToMany(() => Travels, (travels) => travels.vehicle)
  public travels!: Travels[];

  public constructor(init?: Partial<Vehicles>) {
    Object.assign(this, init);
  }
}
